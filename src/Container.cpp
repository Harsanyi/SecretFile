#include "Container.h"

long Container::GetSize()
{
    long size = sizeof(uint64_t);
    for (int f=0;f<files.size();f++)
    {
        size += files[f].GetSize();
    }
    return size;
}
