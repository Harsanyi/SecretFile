#pragma once
#include <cstdint>
#include <string>
#include <vector>

class FileData
{
public:
    FileData() = default;
    FileData(std::string fileName, std::vector<uint8_t> data);
    std::string fileName;
    std::vector<uint8_t> data;
    uint64_t GetSize() const;
};
