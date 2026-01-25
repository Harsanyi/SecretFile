#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "Container.h"
#include "FileData.h"

static const std::string KEY = "CONTAINER";

class BitStream
{
private:
    void Pos2Indexes(const uint64_t pos, int& byteIndex, int& bitIndex);
    uint64_t Indexes2Pos(const int byteIndex, const int bitIndex);
    
public:
    std::vector<uint8_t>& data;
    uint8_t bitDepth = 4;
    uint64_t bitPosition = 0;
    
    BitStream(std::vector<uint8_t>& data, uint8_t bitDepth);

    static uint8_t GetMask(const int start, const int end);
    int GetCapacityInBits(int bitDepth)const;

    static uint8_t ReadBits(const uint8_t byte, const uint8_t start, const uint8_t end);
    static void WriteBits(uint8_t& byte, const uint8_t start, const uint8_t end,const uint8_t value);

    uint8_t ReadBits(const uint8_t count);
    void WriteBits(uint8_t value, const uint8_t count);

    void WriteBytes(const std::vector<uint8_t>& bytes);
    std::vector<uint8_t> ReadBytes(int count);
    
    void WriteUInt64(const uint64_t value);
    uint64_t ReadUInt64();

    void WriteData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> ReadData();

    void WriteString(const std::string& str);
    std::string ReadString();

    void WriteFileData(const FileData& data);
    FileData ReadFileData();

    void WriteContainer(const Container& container);
    Container* ReadContainer();

    bool IsHaveKey();
    void WriteKey();

    bool AtEnd()const;
};
