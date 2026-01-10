#include "SecFile.h"

#include <iostream>

#include "BitStream.h"


void SecFile::WriteDataToImage(Image& img, const std::vector<uint8_t>& data, uint8_t bitDepth)
{
    auto colors = ImageToByteVector(img);
    BitStream bs(colors, bitDepth);
    uint64_t size = data.size();
    bs.WriteUInt64(size);
    bs.WriteBytes(data);
    std::memcpy(img.data, bs.data.data(), colors.size());
}

std::vector<uint8_t> SecFile::ReadDataFromImage(Image& img, uint8_t bitDepth)
{
    auto colors = ImageToByteVector(img);
    BitStream bs(colors, bitDepth);
    uint64_t size = bs.ReadUInt64();
    return  bs.ReadBytes(size*8);
}

std::vector<uint8_t> SecFile::ImageToByteVector(const Image& img)
{
    int byteCount = img.width * img.height * 4;
    uint8_t* colors = static_cast<uint8_t*>(img.data);
    return  std::vector<uint8_t>(colors, colors + byteCount);
}
