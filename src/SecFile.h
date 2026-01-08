#pragma once
#include <vector>
#include "raylib.h"

class SecFile
{
private:
    
public:
    static void WriteDataToImage(Image& img, const std::vector<uint8_t>& data, int bitDepth);
    static std::vector<uint8_t> ReadDataFromImage(Image& img, int bitDepth, int bitCount);
    static std::vector<uint8_t> ImageToByteVector(const Image& img);
};
