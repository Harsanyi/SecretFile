#include "Random.h"

#include <cstring>
#include <ctime>
#include <vector>

uint32_t Random::state = 0;

uint32_t Random::GetUint()
{
    if (state == 0)state = time(NULL);
    Randomize();
    return state;
}

uint32_t Random::GetUint(const int max)
{
    if (max <= 0)return 0;
    return GetUint() % max;
}

uint32_t Random::GetUint(const int min, const int max)
{
    if (min >= max)return max;
    
    int range = max - min;
    return GetUint(range)+min;
}

uint8_t Random::GetByte()
{
    return (uint8_t)(GetUint() & 255);
}

float Random::GetFloat()
{
    return static_cast<float>(GetUint()) / static_cast<float>(UINT32_MAX);
}

std::vector<uint8_t> Random::GetBytes(const long amount)
{
    std::vector<uint8_t> res(amount);
    for (int i = 0; i < amount; i++)
    {
        res[i] = GetByte();
    }
    return res;
}

void Random::Randomize()
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
}
