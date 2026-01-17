#include "FileData.h"

long FileData::GetSize()
{
    long size = sizeof(uint64_t);
    size += fileName.size();
    size += sizeof(uint64_t);
    size += data.size();
    return size;
}
