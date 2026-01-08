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

    static uint8_t ReadBits(const std::vector<uint8_t>& data, int bytePos, int start, int end);
    uint8_t ReadBits(int bytePos, int start, int end) const;
    
    std::vector<uint8_t> ReadBytes(int bitCount, int bitDepth) const;
    
    static void WriteBits(std::vector<uint8_t>& data, int bytePos, int start, int end, uint8_t value);
    void WriteBits(int bytePos, int start, int end, uint8_t value);

    void WriteBytes(int byteIndex, int bitIndex, const std::vector<uint8_t>& bytes, int bitDepth);

    int GetTotalSize(int bitDepth)const;
};
