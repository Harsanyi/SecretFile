#pragma once
#include <cstdint>

class BitStream
{
private:
    static void Pos2Indexes(const int pos, int& byteIndex, int& bitIndex);
    
public:
    uint8_t* data;
    int position = 0;
    
    BitStream(uint8_t* data);

    static uint8_t GetMask(const int start, const int end);
    
    uint8_t ReadBits(const int byteIndex, const int start, const int end);
    void WriteBits(const int byteIndex, const int start, const int end, uint8_t value);
    
    uint8_t* ReadBits(const int amount);
};
