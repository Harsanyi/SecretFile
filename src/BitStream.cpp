#include "BitStream.h"

#include <format>
#include <iostream>
#include <vector>

#include "Random.h"

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

uint8_t BitStream::ReadBits(const int byteIndex, const int start, const int end)
{
    uint8_t mask = GetMask(start, end);
    mask &= data[byteIndex];
    return mask;
}

void BitStream::WriteBits(const int byteIndex, const int start, const int end, uint8_t value)
{
    uint8_t mask = GetMask(start, end);
    value &= mask;
    data[byteIndex] &= ~mask;
    data[byteIndex] |= value;
}

BitStream::BitStream(std::vector<uint8_t>& data) : data(data)
{
    position = 0;
}

void BitStream::WriteBits(const std::vector<uint8_t>& values, const int bits)
{
    int byteIndex = 0;
    int bitIndex = 0;
    Pos2Indexes(position, byteIndex, bitIndex);

    if (byteIndex >= data.size())
    {
        std::cerr << "Byte index " << byteIndex << " is out of bounds" << data.size() << std::endl;
        return;
    }

    int valueIndex = 0;
    while (valueIndex < values.size())
    {
        
    }
}

uint8_t* BitStream::ReadBits(const int amount)
{
    uint8_t* result = new uint8_t[amount];

    int byteIndex, bitIndex;
    Pos2Indexes(position, byteIndex, bitIndex);
    
    ReadBits(byteIndex, bitIndex, amount);
    
    return 0b0;
}
