#include "FileData.h"
#include "Capacity.h"
#include <utility>

FileData::FileData(std::string name, std::vector<uint8_t> bytes)
    : fileName(std::move(name)), data(std::move(bytes)) {}

uint64_t FileData::GetSize() const
{
    return CheckedAdd(16, CheckedAdd(fileName.size(), data.size()));
}
