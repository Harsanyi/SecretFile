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

void BitStream::WriteBits(std::vector<uint8_t> data, const int byteIndex, const int start, const int end, uint8_t value)
{
    uint8_t mask = GetMask(start, end);
    value &= mask;
    data[byteIndex] &= ~mask;
    data[byteIndex] |= value;
}

uint8_t BitStream::ReadBits(const std::vector<uint8_t>& data, const int byteIndex, const int start, const int end)
{
    uint8_t mask = GetMask(start, end);
    mask &= data[byteIndex];
    return mask;
}

uint8_t BitStream::ReadBits(const int byteIndex, const int start, const int end) const
{
    return ReadBits(this->data, byteIndex, start, end);
}

std::vector<uint8_t> BitStream::ReadBits(const int bitCount, const int bitDepth)
{
    int length = bitCount / bitDepth;
    std::vector<uint8_t> result(length);
    int readed = 0;

    while (position < length && readed < bitCount)
    {
        int bytePos, bitPos;
        Pos2Indexes(position, bytePos, bitPos);
        if (bitPos >= 8)
        {
            position++;
            continue;
        }

        if (bytePos >= data.size())break;

        int resBitPos, resBytePos;
        Pos2Indexes(readed, resBytePos, resBitPos);
        if (resBitPos >= 8)
        {
            readed++;
            continue;
        }
        
        int canRead = 8 - std::max(resBitPos, bitPos);
        uint8_t readedVal = ReadBits(bytePos, bitPos, bitPos+canRead);
        if (bitPos > resBitPos)
        {
            readedVal = readedVal >> (bitPos-resBitPos); 
        }
        if (resBitPos > bitPos)
        {
            readedVal = readedVal << (bitPos-resBitPos);
        }
        
        WriteBits(result, resBytePos, resBitPos, resBitPos+canRead, readedVal);

        readed += canRead;
        this->position+=canRead;
    }
        
    return result;
}

void BitStream::WriteBits(const int byteIndex, const int start, const int end, uint8_t value)
{
    WriteBits(this->data, byteIndex, start, end, value);
}

void BitStream::WriteBits(const std::vector<uint8_t>& values, const int bits)
{
    std::vector<uint8_t> sourceData(values);
    BitStream sourceStream(sourceData);
    
    int byteIndex = 0;
    int bitIndex = 0;
    Pos2Indexes(position, byteIndex, bitIndex);

    if (byteIndex >= data.size())
    {
        std::cerr << "Byte index " << byteIndex << " is out of bounds" << data.size() << std::endl;
        return;
    }
    
    while (sourceStream.position < sourceData.size())
    {
        //get writepos
        if (bitIndex < 8-bits)
        {
            position += 8-bits-bitIndex;
            bitIndex = 8-bits;
        }

        //get bits to write
        int bitsToWrite = 8-bitIndex;
        if (bitsToWrite <= 0)
        {
            std::cerr << "Not enough bits!\n";
            return;
        }

        
    }
}
