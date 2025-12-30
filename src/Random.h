#pragma once
#include <cstdint>

static class Random
{
    static uint32_t state;

public:
    static uint32_t GetUint();
    
private:
    static void Randomize();
};
