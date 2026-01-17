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

auto SecFile::GetSizeInBytes(Texture2D* texture, int bitDepth) -> long
{
    return texture->width * texture->height * 4 * bitDepth / 8;
}

void SecFile::WriteDataToTexture(Texture2D& texture, const std::vector<uint8_t>& data, uint8_t bitDepth)
{
    Image img = LoadImageFromTexture(texture);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    WriteDataToImage(img, data, bitDepth);
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
}

Container* SecFile::GetContentFromTexture(Texture2D& texture, uint8_t& bitDepth)
{
    Image img = LoadImageFromTexture(texture);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Container* cont = GetContentFromImage(img, bitDepth);
    UnloadImage(img);
    return cont;
}

Container* SecFile::GetContentFromImage(Image& img, uint8_t bitDepth)
{
    std::vector<uint8_t> imgData = ImageToByteVector(img);
    BitStream bs(imgData,bitDepth);
    return bs.ReadContainer();
}
