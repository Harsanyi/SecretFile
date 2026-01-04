#include "BitStream.h"

#include <format>
#include <iostream>
#include <vector>

#include "Random.h"

BitStream::BitStream(std::vector<uint8_t>& data) : data(data)
{
    position = 0;
}

void BitStream::Pos2Indexes(const int pos, int& byteIndex, int& bitIndex)
{
    byteIndex = pos / 8;
    bitIndex = pos % 8;
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

std::vector<uint8_t> BitStream::ReadBytes(int bitCount, int bitDepth) const
{
    int byteIndex = 0;
    int bitIndex = 0;
    
    if (byteIndex >= data.size())throw std::out_of_range("BitStream::ReadBytes");
    if (bitIndex >= 8)throw std::out_of_range("BitStream::ReadBytes");

    int resultSize = bitCount %8 == 0 ? bitCount / 8 : bitCount / 8 + 1;
    std::vector<uint8_t> result(resultSize);
    int readed = 0;
    int resByteIndex = 0;
    int resBitIndex = 0;
    while (readed < bitCount || byteIndex >= data.size())
    {
        //get bounds
        byteIndex = readed / bitDepth;
        bitIndex = readed % bitDepth;
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

    return result;
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

void BitStream::WriteBytes(const std::vector<uint8_t>& bytes, int bitDepth)
{
    int byteIndex = 0;
    int bitIndex = 0;
    
    int totalSize = GetTotalSize(bitDepth);
    int avaiableSize = totalSize - (byteIndex * bitDepth - (bitIndex > bitDepth ? bitIndex-bitDepth : 0));
    int requireSize = bytes.size() * 8;
    if (avaiableSize < requireSize)
        throw std::out_of_range(std::format("Not enough space in file! avaiable:{} required:{}", avaiableSize, requireSize));

    int srcByteIndex = 0;
    int srcBitIndex = 0;
    int writed = 0;

    auto srcData = bytes;
    BitStream bs(srcData);
    while (writed < requireSize && byteIndex < data.size())
    {
        //get indexes
        byteIndex = writed / bitDepth;
        bitIndex = writed % bitDepth;
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
}

int BitStream::GetTotalSize(int bitDepth) const
{
    return data.size()*bitDepth;
}
