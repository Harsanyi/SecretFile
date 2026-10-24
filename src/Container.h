#pragma once
#include "FileData.h"

class Container
{
public:
    inline static constexpr char Magic[] = "CONTAINER";
    std::vector<FileData> files;
    uint64_t GetSize() const;
};
