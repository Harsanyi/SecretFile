#include "BitStream.h"

#include <format>
#include <iostream>
#include <vector>

#include "Random.h"

BitStream::BitStream(std::vector<uint8_t>& data, std::vector<uint8_t> bitDepths) : data(data)
{
    this->bitPosition = 0;
    this->bitDepths = bitDepths;
}

std::vector<uint8_t> BitStream::GetByteLevelBitDepths(uint8_t bitDepth, uint8_t bytes)
{
    std::vector<uint8_t> result(0);
    while (bitDepth > 8)
    {
        result.push_back(8);
        bitDepth -= 8;
    }
    result.push_back(bitDepth);

    while (result.size() < bytes)
    {
        result.push_back(0);
    }
    
    return result;
}

void BitStream::SetBitDepth(uint8_t bitDepth, uint8_t bytes)
{
    bitDepths = GetByteLevelBitDepths(bitDepth, bytes);
}

uint8_t BitStream::GetBitDepth() const
{
    uint8_t bitDepth = 0;
    for (auto depth : bitDepths)
    {
        bitDepth += depth;
    }
    return bitDepth;
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

uint8_t BitStream::GetBitDepth(const uint64_t index)
{
    return bitDepths[index % bitDepths.size()];
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
    std::vector<uint8_t> result(count);
    for (int b=0; b<count; b++)
    {
        result[b] = ReadBits(8);
    }
    return result;
}

void BitStream::WriteUInt64(const uint64_t value)
{
    std::cout << "Write uint64: " << value << std::endl;
    std::vector<uint8_t> bytes(sizeof(uint64_t));
    std::memcpy(bytes.data(), &value, sizeof(uint64_t));
    WriteBytes(bytes);
}

uint64_t BitStream::ReadUInt64()
{
    auto readed = ReadBytes(sizeof(uint64_t));
    uint64_t result;
    std::memcpy(&result, readed.data(), sizeof(uint64_t));
    std::cout << "Read uint64: " << result << std::endl;
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
    return ReadBytes(size);
}

void BitStream::WriteString(const std::string& str)
{
    uint64_t startPos = bitPosition;
    std::cout << "Write string: " << str << std::endl;
    WriteUInt64(str.size());
    std::vector<uint8_t> bytes(str.begin(),str.end());
    WriteBytes(bytes);

    bitPosition = startPos;
    std::string read = ReadString();
    std::cout << "read back:" << read << std::endl;
}

std::string BitStream::ReadString()
{
    uint64_t size = ReadUInt64();
    std::vector<uint8_t> bytes = ReadBytes(size);
    std::string result(bytes.begin(), bytes.end());
    //std::string result = std::string(bytes.begin(),bytes.end());
    std::cout << "Read string: " << result.c_str() << std::endl;
    return result;
}

void BitStream::WriteFileData(const FileData& data)
{
    WriteString(data.fileName);
    WriteData(data.data);
}

FileData BitStream::ReadFileData()
{
    std::string fileName = ReadString();
    std::vector<uint8_t> data = ReadData();
    FileData file(fileName, data); 
    return file;
}

void BitStream::WriteContainer(const Container& container)
{
    WriteKey();
    WriteUInt64((uint64_t)container.files.size());
    std::cout << "Write files:" << container.files.size() << std::endl;
    for (int f=0;f<container.files.size();f++)
    {
        WriteFileData(container.files[f]);
    }
}

Container* BitStream::ReadContainer()
{
    if (!IsHaveKey())return nullptr;
    
    int size = ReadUInt64();
    Container* result = new Container();
    std::cout << "Found files: " << size << std::endl; 
    for (int f=0;f<size;f++)
    {
        FileData data = ReadFileData();
        result->files.push_back(std::move(data));
    }
    return result;
}

bool BitStream::IsHaveKey()
{
    auto bytes = ReadBytes(KEY.length());
    std::string readedKey = std::string(bytes.begin(),bytes.end());
    std::cout << "Readed key: " << readedKey << std::endl;
    
    return readedKey == KEY;
}

void BitStream::WriteKey()
{
    std::vector<uint8_t> bytes(KEY.length());
    std::memcpy(bytes.data(), KEY.data(), KEY.length());
    WriteBytes(bytes);

    std::cout << "Key written!" << std::endl;
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
    if (count > 8)throw std::exception("You cannot read more than 8 bit at a time!");
    
    uint8_t result = 0;
    int readed = 0;
    while (readed < count)
    {
        int byteIndex, bitIndex;
        Pos2Indexes(bitPosition, byteIndex, bitIndex);

        uint8_t bitDepth = GetBitDepth(bitIndex);
        while (bitIndex >= bitDepth)
        {
            byteIndex++;
            bitIndex -= bitDepth;
            bitDepth = GetBitDepth(byteIndex);
        }

        int readable = std::min(bitDepth-bitIndex, count-readed);
        uint8_t value = ReadBits(data[byteIndex], bitIndex, bitIndex + readable-1);
        value >>= bitIndex;
        value <<= readed;
        result |= value;
        readed += readable;
        bitIndex += readable;

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
        uint8_t bitDepth = GetBitDepth(bitIndex);
        while (bitIndex >= bitDepth)
        {
            byteIndex++;
            bitIndex -= bitDepth;
            bitDepth = GetBitDepth(byteIndex);
        }

        int writeable = std::min(bitDepth - bitIndex, count - writed);
        WriteBits(data[byteIndex], bitIndex, bitIndex + writeable - 1, value);
        value >>= writeable;
        writed += writeable;
        bitIndex += writeable;

        bitPosition = Indexes2Pos(byteIndex, bitIndex);
    }
}
