#include "Container.h"
#include "Capacity.h"

uint64_t Container::GetSize() const
{
    uint64_t size = sizeof(Magic) - 1 + sizeof(uint64_t);
    for (const auto& file : files) size = CheckedAdd(size, file.GetSize());
    return size;
}
