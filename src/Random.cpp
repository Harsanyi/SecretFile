#include "Random.h"

#include <ctime>

uint32_t Random::state = 0;

uint32_t Random::GetUint()
{
    if (state == 0)state = time(NULL);
    Randomize();
    return state;
}

void Random::Randomize()
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
}
