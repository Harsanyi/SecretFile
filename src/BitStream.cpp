#include "BitStream.h"

#include <format>
#include <iostream>
#include <vector>

#include "Random.h"

BitStream::BitStream(std::vector<uint8_t>& data, uint8_t bitDepth) : data(data)
{
    position = 0;
    this->bitDepth = bitDepth;
}

void BitStream::Pos2Indexes(const uint64_t pos, int& byteIndex, int& bitIndex)
{
    byteIndex = pos / 8;
    bitIndex = pos % 8;
}

uint64_t BitStream::Indexes2Pos(const int byteIndex, const int bitIndex)
{
    return byteIndex * 8 + bitIndex;
}

uint8_t BitStream::GetMask(const int start, const int end)
{
    uint8_t result = 0b0;
    for (int i=0; i<8; i++)
    {
        if (i < start || i > end)continue;
        result |= (1 << i);
    }

    return result;
}

uint8_t BitStream::ReadBits(const std::vector<uint8_t>& data, int bytePos, int start, int end)
{
    if (bytePos >= data.size())throw std::out_of_range(std::format("bytepos:{}>=datasize:{}", bytePos, data.size()));
    if (start < 0 || start > end)throw std::out_of_range("BitStream::ReadBits");
    if (end >= 8)throw std::out_of_range("BitStream::ReadBits");
    
    uint8_t mask = GetMask(start, end);
    return data[bytePos] & mask;
}

uint8_t BitStream::ReadBits(int bytePos, int start, int end) const
{
    return ReadBits(this->data, bytePos, start, end);
}

std::vector<uint8_t> BitStream::ReadBytes(int byteIndex, int bitIndex, uint64_t bitCount)
{
    if (byteIndex >= data.size())throw std::out_of_range("BitStream::ReadBytes");
    if (bitIndex >= 8)throw std::out_of_range("BitStream::ReadBytes");

    int resultSize = bitCount %8 == 0 ? bitCount / 8 : bitCount / 8 + 1;
    std::vector<uint8_t> result(resultSize);
    int readed = 0;
    int resByteIndex = 0;
    int resBitIndex = 0;
    int occupied = byteIndex * 8;
    while (readed < bitCount || byteIndex >= data.size())
    {
        //get bounds
        byteIndex = (readed + occupied) / bitDepth;
        bitIndex = (readed + occupied) % bitDepth;
        resByteIndex = readed / 8;
        resBitIndex = readed % 8;
        int canRead = 8 - std::max(resBitIndex, bitIndex);

        //read data
        uint8_t byte = ReadBits(byteIndex, bitIndex, bitIndex+(canRead-1));

        //offset data
        if (resBitIndex < bitIndex)
        {
            byte >>= bitIndex-resBitIndex;
        }
        if (resBitIndex > bitIndex)
        {
            byte <<= resBitIndex-bitIndex;
        }

        //write data
        WriteBits(result, resByteIndex, resBitIndex, resBitIndex+(canRead-1), byte);

        //apply changes
        readed += canRead;
    }

    position = Indexes2Pos(byteIndex, bitIndex);
    
    return result;
}

std::vector<uint8_t> BitStream::ReadBytes(uint64_t bitCount)
{
    int byteIndex, bitIndex;
    Pos2Indexes(position, byteIndex, bitIndex);
    return ReadBytes(byteIndex, bitIndex, bitCount);
}

void BitStream::WriteBits(std::vector<uint8_t>& data, int bytePos, int start, int end, uint8_t value)
{
    if (bytePos >= data.size())throw std::out_of_range(std::format("BitStream::WriteBits bytepos:{} >= data.size:{}", bytePos, data.size()));
    if (start < 0 || start > end)throw std::out_of_range(std::format("BitStream::WriteBits start:{} end:{}", start, end));
    if (end >= 8)throw std::out_of_range(std::format("BitStream::WriteBits end:{} is >= 8!",end));

    uint8_t mask = GetMask(start, end);
    value &= mask;
    data[bytePos] &= ~mask;
    data[bytePos] |= value;
}

void BitStream::WriteBits(int bytePos, int start, int end, uint8_t value)
{
    return WriteBits(this->data, bytePos, start, end, value);
}

void BitStream::WriteBytes(int byteIndex, int bitIndex, const std::vector<uint8_t>& bytes)
{
    bitIndex = std::max(bitIndex, 8-bitDepth);

    int totalSize = data.size()*8;
    int occupied = byteIndex*8;
    int avaiableSize = totalSize-occupied;
    int requireSize = bytes.size() * 8;
    if (avaiableSize < requireSize)
        throw std::out_of_range(std::format("Not enough space in file! avaiable:{} required:{}", avaiableSize, requireSize));

    int srcByteIndex = 0;
    int srcBitIndex = 0;
    int writed = 0;

    auto srcData = bytes;
    BitStream bs(srcData, bitDepth);
    while (writed < requireSize && byteIndex < data.size())
    {
        //get indexes
        byteIndex = (writed+occupied) / bitDepth;
        bitIndex = (writed+occupied) % bitDepth;
        srcByteIndex = writed / 8;
        srcBitIndex = writed % 8;

        //how much can write?
        int canWrite = std::min(8-bitIndex,8-srcBitIndex);

        //read val
        uint8_t val = bs.ReadBits(srcByteIndex, srcBitIndex, srcBitIndex+(canWrite-1));

        //offset
        if (bitIndex < srcBitIndex)
        {
            val >>= srcBitIndex-bitIndex;
        }
        if (srcBitIndex < bitIndex)
        {
            val <<= bitIndex-srcBitIndex;
        }

        //write data
        WriteBits(byteIndex, bitIndex, bitIndex+(canWrite-1), val);

        //apply changes
        writed += canWrite;
    }

    position = Indexes2Pos(byteIndex, bitIndex);
}

void BitStream::WriteBytes(const std::vector<uint8_t>& bytes)
{
    int byteIndex, bitIndex;
    Pos2Indexes(position, byteIndex, bitIndex);
    WriteBytes(byteIndex, bitIndex, bytes);
}

void BitStream::WriteUInt64(const uint64_t value)
{
    std::vector<uint8_t> bytes(sizeof(uint64_t));
    std::memcpy(bytes.data(), &value, sizeof(uint64_t));
    WriteBytes(bytes);
}

uint64_t BitStream::ReadUInt64()
{
    auto readed = ReadBytes(sizeof(uint64_t)*8);
    uint64_t result;
    std::memcpy(&result, readed.data(), sizeof(uint64_t));
    return result;
}

void BitStream::WriteData(const std::vector<uint8_t>& data)
{
    WriteUInt64(data.size());
    WriteBytes(data);
}

std::vector<uint8_t> BitStream::ReadData()
{
    long size = ReadUInt64();
    return ReadBytes(size*8);
}

void BitStream::WriteString(const std::string& str)
{
    WriteUInt64(str.size());
    std::vector<uint8_t> bytes(str.begin(),str.end());
    WriteBytes(bytes);
}

std::string BitStream::ReadString()
{
    long size = ReadUInt64();
    std::vector<uint8_t> bytes = ReadBytes(size*8);
    return std::string(bytes.begin(),bytes.end());
}

void BitStream::WriteFileData(const FileData& data)
{
    WriteString(data.fileName);
    WriteData(data.data);
}

FileData BitStream::ReadFileData()
{
    return FileData(ReadString(),ReadData());
}

void BitStream::WriteContainer(const Container& container)
{
    WriteKey();
    WriteUInt64(container.files.size());
    for (int f=0;f<container.files.size();f++)
    {
        WriteFileData(container.files[f]);
    }
}

Container* BitStream::ReadContainer()
{
    if (!HaveKey())
    {
        return nullptr;
    }
    
    int size = ReadUInt64();
    Container* result = new Container();
    for (int f=0;f<size;f++)
    {
        result->files.push_back(ReadFileData());
    }
    return result;
}

bool BitStream::HaveKey()
{
    auto bytes = ReadBytes(0,0, KEY.length()*8);
    std::string readedKey = std::string(bytes.begin(),bytes.end());
    return readedKey == KEY;
}

void BitStream::WriteKey()
{
    std::vector<uint8_t> bytes(KEY.length()*8);
    std::memcpy(bytes.data(), KEY.data(), KEY.length()*8);
    WriteBytes(bytes);
}

int BitStream::GetTotalSize(int bitDepth) const
{
    return data.size()*bitDepth;
}
