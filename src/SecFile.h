#pragma once
#include <vector>

#include "BitStream.h"
#include "raylib.h"

class SecFile
{
public:
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    
    static void WriteDataToImage(Image& img, const std::vector<uint8_t>& data, uint8_t bitDepth);
    static std::vector<uint8_t> ReadDataFromImage(Image& img, uint8_t bitDepth);
    static std::vector<uint8_t> ImageToByteVector(Image& img);
    static long GetSizeInBytes(int width, int height, uint8_t bitDepth);
    static void WriteDataToTexture(Texture2D& texture, const std::vector<uint8_t>& data, uint8_t bitDepth);
    static Container* ReadContainerFromTexture(Texture2D& texture, uint8_t bitDepth);
    static Container* ReadContainerFromImage(Image& img, uint8_t bitDepth);
    static void WriteContainerToImage(Container& container, Image& img, uint8_t bitDepth);

    Texture2D* loadedTexture = nullptr;
    std::string loadedTextureName;
    uint8_t bitDepth = 1;
    Container* container = nullptr;

    void LoadContainer();
    void AddFileFromPath(std::string path);
    void SaveImage();
    void RemoveFileAt(const int index);
    void ExportFileAt(const int index);
};
