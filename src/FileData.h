#pragma once
#include <string>
#include <vector>

class FileData
{
public:
    std::string fileName;
    std::vector<uint8_t> data;

    long GetSize();
};
