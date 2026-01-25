#pragma once
#include <cstdint>
#include <vector>

class Random
{
    static uint32_t state;

public:
    static uint32_t GetUint();
    static uint32_t GetUint(const int max);
    static uint32_t GetUint(const int min, const int max);
    static uint8_t GetByte();
    static float GetFloat();
    
    static std::vector<uint8_t> GetBytes(const long amount);
    
private:
    static void Randomize();
};
