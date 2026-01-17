#pragma once
#include <vector>
#include "raylib.h"

class SecFile
{
private:
    
public:
    static void WriteDataToImage(Image& img, const std::vector<uint8_t>& data, uint8_t bitDepth);
    static std::vector<uint8_t> ReadDataFromImage(Image& img, uint8_t bitDepth);
    static std::vector<uint8_t> ImageToByteVector(const Image& img);
    static long GetSizeInBytes(Texture2D* texture, int bitDepth);
    static void WriteDataToTexture(Texture2D& texture, const std::vector<uint8_t>& data, uint8_t bitDepth);
};
