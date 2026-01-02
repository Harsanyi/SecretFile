#pragma once
#include <cstdint>
#include <vector>

class BitStream
{
private:
    static void Pos2Indexes(const int pos, int& byteIndex, int& bitIndex);
    
public:
    std::vector<uint8_t>& data;
    int position = 0;
    
    BitStream(std::vector<uint8_t>& data);

    static uint8_t GetMask(const int start, const int end);

    void WriteBits(const int byteIndex, const int start, const int end, uint8_t value);
    uint8_t ReadBits(const int byteIndex, const int start, const int end);
    void WriteBits(const std::vector<uint8_t>& values, const int bits);
    uint8_t* ReadBits(const int amount);
};
