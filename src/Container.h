#pragma once
#include "FileData.h"

class Container
{
public:
    std::vector<FileData> files;

    long GetSize();
};
