#include "BitStream.h"

#include <format>
#include <iostream>
#include <vector>

#include "Random.h"

BitStream::BitStream(std::vector<uint8_t>& data, uint8_t bitDepth) : data(data)
{
    this->bitPosition = 0;
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

void BitStream::WriteBytes(const std::vector<uint8_t>& bytes)
{
    for (int b=0; b<bytes.size(); b++)
    {
        WriteBits(bytes[b], 8);
    }
}

std::vector<uint8_t> BitStream::ReadBytes(int count)
{
    std::cout << "Implement" << std::endl;
    return std::vector<uint8_t>(count);
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
    auto bytes = ReadBytes(KEY.length());
    std::string readedKey = std::string(bytes.begin(),bytes.end());
    return readedKey == KEY;
}

void BitStream::WriteKey()
{
    std::vector<uint8_t> bytes(KEY.length()*8);
    std::memcpy(bytes.data(), KEY.data(), KEY.length()*8);
    WriteBytes(bytes);
}

bool BitStream::AtEnd() const
{
    return bitPosition >= data.size()*8;
}

int BitStream::GetCapacityInBits(int bitDepth) const
{
    return data.size()*bitDepth;
}

uint8_t BitStream::ReadBits(const uint8_t byte, const uint8_t start, const uint8_t end)
{
    if (end >= 8)throw std::exception(std::format("Out of byte index! {}", end).c_str());
    if (start > end)throw std::exception(std::format("start:{} must be less or equal to end:{}", start,end).c_str());

    uint8_t mask = GetMask(start, end);
    return byte & mask;
}

void BitStream::WriteBits(uint8_t& byte, const uint8_t start, const uint8_t end, const uint8_t value)
{
    if (end >= 8)throw std::exception(std::format("Out of byte index! {}", end).c_str());
    if (start > end)throw std::exception(std::format("start:{} must be less or equal to end:{}", start,end).c_str());
    uint8_t mask = GetMask(start, end);
    byte &= ~mask;
    byte |= (value & mask);
}

uint8_t BitStream::ReadBits(const uint8_t count)
{
    if (count > 8)throw std::exception("Cannot read more than 8 bits at a time!");
    
    int readed = 0;
    uint8_t result = 0b0;
    while (readed < count)
    {
        int byteIndex, bitIndex;
        Pos2Indexes(bitPosition, byteIndex, bitIndex);
        bitIndex = std::max(bitIndex, 8-bitDepth);

        while (bitIndex >= 8)
        {
            byteIndex++;
            bitIndex -= 8;
        }

        if (byteIndex >= data.size())throw std::exception(std::format("Out of byte index! {}", byteIndex).c_str());

        
        int canRead = std::min((int)count, 8-bitIndex);
        uint8_t readVal = ReadBits(byteIndex, bitIndex, bitIndex+canRead);
        readVal >>= bitIndex;
        result |= (readVal << readed);
        readed += canRead;

        bitIndex += canRead;
        bitPosition = Indexes2Pos(byteIndex, bitIndex);
    }

    return result;
}

//Value must be offsetted to start with the count
void BitStream::WriteBits(uint8_t value, const uint8_t count)
{
    if (count > 8)throw std::exception("Cannot write more than 8 bits at a time!");
    
    uint8_t writed = 0;
    while (writed < count)
    {
        int byteIndex, bitIndex;
        Pos2Indexes(bitPosition, byteIndex, bitIndex);
        while (bitIndex >= bitDepth)
        {
            byteIndex++;
            bitIndex -= bitDepth;
        }

        int writeable = std::min(bitDepth - bitIndex, count - writed);
        WriteBits(data[byteIndex], bitIndex, bitIndex + writeable - 1, value);
        value >>= writeable;
        writed += writeable;
        bitIndex += writeable;

        bitPosition = Indexes2Pos(byteIndex, bitIndex);
    }
}
