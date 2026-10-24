#include "BitStream.h"
#include "Capacity.h"
#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>

BitStream::BitStream(std::vector<uint8_t>& carrier, std::vector<uint8_t> depths)
    : data(carrier), bitDepths(std::move(depths))
{
    if (bitDepths.empty() || bitDepths.size() > 32)
        throw std::invalid_argument("A carrier needs a byte-depth pattern");
    for (auto depth : bitDepths)
        if (depth > 8) throw std::invalid_argument("A byte cannot carry more than eight bits");
    const uint64_t bitsPerElement = std::accumulate(bitDepths.begin(), bitDepths.end(), uint64_t{0});
    if (bitsPerElement == 0) throw std::invalid_argument("Zero bit depth is preview-only");
    capacityBits = CheckedMultiply(data.size() / bitDepths.size(), bitsPerElement);
    for (std::size_t i = 0; i < data.size() % bitDepths.size(); ++i)
        capacityBits = CheckedAdd(capacityBits, bitDepths[i]);
}

std::vector<uint8_t> BitStream::GetByteLevelBitDepths(uint8_t depth, uint8_t bytes)
{
    if (bytes == 0 || depth > unsigned(bytes) * 8)
        throw std::invalid_argument("Bit depth exceeds carrier element width");
    std::vector<uint8_t> result(bytes, 0);
    for (auto& byteDepth : result)
    {
        byteDepth = std::min<uint8_t>(depth, 8);
        depth -= byteDepth;
    }
    return result;
}

void BitStream::RequireBits(uint64_t count) const
{
    if (count > RemainingBits()) throw std::out_of_range("Payload exceeds remaining carrier capacity");
}

void BitStream::NormalizeCursor()
{
    // Depth belongs to the physical byte, including zero-depth bytes in a sample/float.
    while (byteIndex < data.size() && bitIndex >= bitDepths[byteIndex % bitDepths.size()])
    {
        ++byteIndex;
        bitIndex = 0;
    }
    if (byteIndex >= data.size()) throw std::out_of_range("Carrier cursor is at the end");
}

uint8_t BitStream::GetMask(int start, int end)
{
    if (start < 0 || end > 7 || start > end)
        throw std::invalid_argument("Invalid byte bit range");
    return static_cast<uint8_t>(((1u << (end - start + 1)) - 1u) << start);
}

uint8_t BitStream::ReadBits(uint8_t byte, uint8_t start, uint8_t end)
{
    return byte & GetMask(start, end);
}

void BitStream::WriteBits(uint8_t& byte, uint8_t start, uint8_t end, uint8_t value)
{
    const auto mask = GetMask(start, end);
    byte = static_cast<uint8_t>((byte & ~mask) | (value & mask));
}

uint8_t BitStream::ReadBits(uint8_t count)
{
    if (count > 8) throw std::invalid_argument("Read at most eight bits at a time");
    RequireBits(count);
    uint8_t result = 0;
    for (uint8_t read = 0; read < count;)
    {
        NormalizeCursor();
        const auto depth = bitDepths[byteIndex % bitDepths.size()];
        const auto take = std::min<uint8_t>(depth - bitIndex, count - read);
        const auto chunk = (data[byteIndex] >> bitIndex) & ((1u << take) - 1u);
        result |= static_cast<uint8_t>(chunk << read);
        bitIndex += take;
        read += take;
    }
    positionBits += count;
    return result;
}

void BitStream::WriteBits(uint8_t value, uint8_t count)
{
    if (count > 8) throw std::invalid_argument("Write at most eight bits at a time");
    RequireBits(count);
    for (uint8_t written = 0; written < count;)
    {
        NormalizeCursor();
        const auto depth = bitDepths[byteIndex % bitDepths.size()];
        const auto take = std::min<uint8_t>(depth - bitIndex, count - written);
        const auto chunk = static_cast<uint8_t>((value >> written) << bitIndex);
        WriteBits(data[byteIndex], bitIndex, bitIndex + take - 1, chunk);
        bitIndex += take;
        written += take;
    }
    positionBits += count;
}

void BitStream::WriteBytes(const std::vector<uint8_t>& bytes)
{
    RequireBits(CheckedMultiply(bytes.size(), 8));
    for (auto byte : bytes) WriteBits(byte, 8);
}

std::vector<uint8_t> BitStream::ReadBytes(uint64_t count)
{
    if (count > RemainingBytes() || count > std::vector<uint8_t>().max_size())
        throw std::out_of_range("Invalid embedded byte length");
    std::vector<uint8_t> result(static_cast<std::size_t>(count));
    for (auto& byte : result) byte = ReadBits(8);
    return result;
}

void BitStream::WriteUInt64(uint64_t value)
{
    RequireBits(64);
    for (unsigned i = 0; i < 8; ++i) WriteBits(static_cast<uint8_t>(value >> (i * 8)), 8);
}

uint64_t BitStream::ReadUInt64()
{
    RequireBits(64);
    uint64_t value = 0;
    for (unsigned i = 0; i < 8; ++i) value |= uint64_t(ReadBits(8)) << (i * 8);
    return value;
}

void BitStream::WriteData(const std::vector<uint8_t>& bytes)
{
    RequireBits(CheckedMultiply(CheckedAdd(8, bytes.size()), 8));
    WriteUInt64(bytes.size());
    WriteBytes(bytes);
}

std::vector<uint8_t> BitStream::ReadData() { return ReadBytes(ReadUInt64()); }

void BitStream::WriteString(const std::string& text)
{
    WriteData(std::vector<uint8_t>(text.begin(), text.end()));
}

std::string BitStream::ReadString()
{
    const auto bytes = ReadData();
    return std::string(bytes.begin(), bytes.end());
}

void BitStream::WriteFileData(const FileData& file)
{
    WriteString(file.fileName);
    WriteData(file.data);
}

FileData BitStream::ReadFileData()
{
    auto name = ReadString();
    auto bytes = ReadData();
    return FileData(std::move(name), std::move(bytes));
}

void BitStream::WriteContainer(const Container& container)
{
    RequireBits(CheckedMultiply(container.GetSize(), 8));
    WriteKey();
    WriteUInt64(container.files.size());
    for (const auto& file : container.files) WriteFileData(file);
}

Container* BitStream::ReadContainer()
{
    if (!IsHaveKey()) return nullptr;
    const auto count = ReadUInt64();
    // Every entry has at least two uint64 lengths, even when both byte arrays are empty.
    if (count > RemainingBytes() / 16) throw std::runtime_error("Invalid embedded file count");
    auto result = std::make_unique<Container>();
    for (uint64_t i = 0; i < count; ++i) result->files.push_back(ReadFileData());
    return result.release();
}

bool BitStream::IsHaveKey()
{
    if (RemainingBytes() < KEY.size()) return false;
    const auto bytes = ReadBytes(KEY.size());
    return std::string(bytes.begin(), bytes.end()) == KEY;
}

void BitStream::WriteKey() { WriteBytes(std::vector<uint8_t>(KEY.begin(), KEY.end())); }
