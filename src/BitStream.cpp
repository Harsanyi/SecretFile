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
    if (bytePos >= data.size())throw std::out_of_range("BitStream::ReadBits");
    if (start < 0 || start >= end)throw std::out_of_range("BitStream::ReadBits");
    if (end >= 8)throw std::out_of_range("BitStream::ReadBits");
    
    uint8_t mask = GetMask(start, end);
    return data[bytePos] && mask;
}

uint8_t BitStream::ReadBits(int bytePos, int start, int end) const
{
    return ReadBits(this->data, bytePos, start, end);
}

std::vector<uint8_t> BitStream::ReadBytes(int bytePos, int bitPos, int bitCount, int bitDepth) const
{
    if (bytePos >= data.size())throw std::out_of_range("BitStream::ReadBytes");
    if (bitPos >= 8)throw std::out_of_range("BitStream::ReadBytes");

    std::vector<uint8_t> result;
    int readed = 0;
    int resByte = 0;
    int resBit = 0;
    while (readed < bitCount || bytePos >= data.size())
    {
        //get bounds
        bitPos = std::min(bitPos, 8-bitDepth);
        int canRead = 8-bitPos;
        canRead = std::min(canRead, 8-resBit);

        //read data
        uint8_t byte = ReadBits(bytePos, bitPos, bitPos+canRead);

        //offset data
        if (resBit < bitPos)
        {
            byte >>= bitPos-resBit;
        }
        if (resBit > bitPos)
        {
            byte <<= resBit-bitPos;
        }

        //write data
        WriteBits(result, resByte, resBit, resBit+canRead, byte);

        //apply changes
        readed += canRead;
        resBit += canRead;
        if (resBit >= 8)
        {
            resByte++;
            resBit -= 8;
        }
        bitPos += canRead;
        if (bitPos >= 8)
        {
            bytePos++;
            bitPos -= 8;
        }
    }

    return result;
}

void BitStream::WriteBits(std::vector<uint8_t>& data, int bytePos, int start, int end, uint8_t value)
{
    if (bytePos >= data.size())throw std::out_of_range(std::format("BitStream::WriteBits bytepos:{} >= data.size:{}", bytePos, data.size()));
    if (start < 0 || start >= end)throw std::out_of_range(std::format("BitStream::WriteBits start:{} end:{}", start, end));
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

void BitStream::WriteBytes(int bytePos, int bitPos, const std::vector<uint8_t>& bytes, int bitDepth)
{
    int totalSize = GetTotalSize(bitDepth);
    int avaiableSize = totalSize - (bytePos * bitDepth - (bitPos > bitDepth ? bitPos-bitDepth : 0));
    int requireSize = bytes.size() * 8;
    if (avaiableSize < requireSize)
        throw std::out_of_range(std::format("Not enough space in file! avaiable:{} required:{}", avaiableSize, requireSize));

    int srcBytePos = 0;
    int srcBitPos = 0;
    int writed = 0;

    auto srcData = bytes;
    BitStream bs(srcData);
    while (writed < bytes.size()*8 && bytePos < data.size())
    {
        if (bitPos >= 8)
        {
            bitPos = 0;
            bytePos++;
            continue;
        }
        
        bitPos = std::max(bitPos, 8-bitDepth);
        int canWrite = std::min(8-bitPos,8-srcBitPos);

        //read val
        uint8_t val = bs.ReadBits(bytePos, srcBitPos, srcBitPos+canWrite);

        //offset
        if (bitPos < srcBitPos)
        {
            val >>= srcBitPos-bitPos;
        }
        if (srcBitPos < bitPos)
        {
            val <<= bitPos-srcBitPos;
        }

        //write data
        WriteBits(bytePos, bitPos, bitPos+canWrite, val);

        //apply changes
        bitPos+=canWrite;
        if (bitPos >= 8)
        {
            bytePos++;
            bitPos = 0;
        }
        srcBitPos += canWrite;
        if (srcBitPos >= 8)
        {
            srcBitPos=0;
            srcBytePos++;
        }
        writed += canWrite;
    }
}

int BitStream::GetTotalSize(int bitDepth) const
{
    return data.size()*bitDepth;
}
