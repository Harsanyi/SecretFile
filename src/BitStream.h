#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "Container.h"
#include "FileData.h"

class BitStream
{
private:
    static void Pos2Indexes(const uint64_t pos, int& byteIndex, int& bitIndex);
    static uint64_t Indexes2Pos(const int byteIndex, const int bitIndex);
    const std::string KEY = "CONTAINER";
    
public:
    std::vector<uint8_t>& data;
    uint64_t position = 0;
    uint8_t bitDepth = 4;
    
    BitStream(std::vector<uint8_t>& data, uint8_t bitDepth);

    static uint8_t GetMask(const int start, const int end);
    int GetTotalSize(int bitDepth)const;

    static uint8_t ReadBits(const std::vector<uint8_t>& data, int bytePos, int start, int end);
    static void WriteBits(std::vector<uint8_t>& data, int bytePos, int start, int end, uint8_t value);
    
    uint8_t ReadBits(int bytePos, int start, int end) const;
    void WriteBits(int bytePos, int start, int end, uint8_t value);
    
    std::vector<uint8_t> ReadBytes(int byteIndex, int bitIndex, uint64_t bitCount);
    void WriteBytes(int byteIndex, int bitIndex, const std::vector<uint8_t>& bytes);
    
    std::vector<uint8_t> ReadBytes(uint64_t bitCount);
    void WriteBytes(const std::vector<uint8_t>& bytes);

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

    bool HaveKey();
    void WriteKey();
};
