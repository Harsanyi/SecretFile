#include "SecFile.h"

#include <iostream>

#include "BitStream.h"


void SecFile::WriteDataToImage(Image& img, const std::vector<uint8_t>& data, int bitDepth)
{
    auto colors = ImageToByteVector(img);
    BitStream bs(colors);
    bs.WriteBytes(0, 0, data, bitDepth);
    std::memcpy(img.data, bs.data.data(), colors.size());
}

std::vector<uint8_t> SecFile::ReadDataFromImage(Image& img, int bitDepth, int bitCount)
{
    auto colors = ImageToByteVector(img);
    BitStream bs(colors);
    return  bs.ReadBytes(bitCount, bitDepth);
}

std::vector<uint8_t> SecFile::ImageToByteVector(const Image& img)
{
    int byteCount = img.width * img.height * 4;
    uint8_t* colors = static_cast<uint8_t*>(img.data);
    return  std::vector<uint8_t>(colors, colors + byteCount);
}
