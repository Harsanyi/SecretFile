#pragma once
#include <vector>

#include "BitStream.h"
#include "raylib.h"

class SecFile
{
private:
    
public:
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    
    static void WriteDataToImage(Image& img, const std::vector<uint8_t>& data, uint8_t bitDepth);
    static std::vector<uint8_t> ReadDataFromImage(Image& img, uint8_t bitDepth);
    static std::vector<uint8_t> ImageToByteVector(const Image& img);
    static long GetSizeInBytes(Texture2D* texture, int bitDepth);
    static void WriteDataToTexture(Texture2D& texture, const std::vector<uint8_t>& data, uint8_t bitDepth);
    static Container* GetContentFromTexture(Texture2D& texture, uint8_t& bitDepth);
    static Container* GetContentFromImage(Image& img, uint8_t bitDepth);

    Texture2D* loadedTexture = nullptr;
    uint8_t bitDepth = 1;
    Container* container = nullptr;

    void LoadContainer();
    void AddFileFromPath(std::string path);
};
