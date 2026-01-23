#include "FileData.h"

FileData::FileData(const std::string& fileName, const std::vector<uint8_t>& data)
{
    this->fileName = fileName;
    this->data = data;
}

long FileData::GetSize()
{
    long size = sizeof(uint64_t);
    size += fileName.size();
    size += sizeof(uint64_t);
    size += data.size();
    return size;
}
