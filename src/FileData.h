#pragma once
#include <string>
#include <vector>

class FileData
{
public:
    FileData() = default;
    FileData(const std::string& fileName, const std::vector<uint8_t>& data);
    ~FileData() = default;
    
    std::string fileName;
    std::vector<uint8_t> data;

    long GetSize();
};
