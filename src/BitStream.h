#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Container.h"

inline const std::string KEY = Container::Magic;

class BitStream
{
    std::vector<uint8_t>& data;
    std::vector<uint8_t> bitDepths;
    std::size_t byteIndex = 0;
    uint8_t bitIndex = 0;
    uint64_t capacityBits = 0;
    uint64_t positionBits = 0;

    void NormalizeCursor();
    void RequireBits(uint64_t count) const;

public:
    BitStream(std::vector<uint8_t>& data, std::vector<uint8_t> bitDepths);
    static std::vector<uint8_t> GetByteLevelBitDepths(uint8_t bitDepth, uint8_t bytes);
    uint64_t RemainingBits() const { return capacityBits - positionBits; }
    uint64_t RemainingBytes() const { return RemainingBits() / 8; }
    bool AtEnd() const { return RemainingBits() == 0; }

    static uint8_t GetMask(int start, int end);
    static uint8_t ReadBits(uint8_t byte, uint8_t start, uint8_t end);
    static void WriteBits(uint8_t& byte, uint8_t start, uint8_t end, uint8_t value);
    uint8_t ReadBits(uint8_t count);
    void WriteBits(uint8_t value, uint8_t count);
    void WriteBytes(const std::vector<uint8_t>& bytes);
    std::vector<uint8_t> ReadBytes(uint64_t count);
    void WriteUInt64(uint64_t value);
    uint64_t ReadUInt64();
    void WriteData(const std::vector<uint8_t>& bytes);
    std::vector<uint8_t> ReadData();
    void WriteString(const std::string& text);
    std::string ReadString();
    void WriteFileData(const FileData& file);
    FileData ReadFileData();
    void WriteContainer(const Container& container);
    Container* ReadContainer();
    bool IsHaveKey();
    void WriteKey();
};
