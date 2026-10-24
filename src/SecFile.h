#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "BitStream.h"
#include "raylib.h"

class SecFile
{
public:
    static std::vector<uint8_t> ImageToByteVector(Image& image);
    static void NormalizeWave(Wave& wave);
    static Container* ReadContainerFromTexture(const Texture2D& texture, uint8_t bitDepth);
    static Container* ReadContainerFromImage(Image& image, uint8_t bitDepth);
    static Container* ReadContainerFromWave(Wave& wave, uint8_t bitDepth);
    static Container* ReadContainerFromModel(const Model& model, uint8_t bitDepth);
    static void WriteContainerToImage(const Container& container, Image& image, uint8_t bitDepth);
    static void WriteContainerToWave(const Container& container, Wave& wave, uint8_t bitDepth);
    static void WriteContainerToModel(const Container& container, Model& model, uint8_t bitDepth);

    Texture2D* loadedTexture = nullptr;
    Wave* loadedWave = nullptr;
    Model* loadedModel = nullptr;
    std::filesystem::path loadedFilePath;
    std::string status;
    uint8_t bitDepth = 1;
    Container* container = nullptr;

    ~SecFile();
    void LoadContainer();
    void AddFileFromPath(const std::string& path);
    void SaveLoaded();
    void RemoveFileAt(std::size_t index);
    void ExportFileAt(std::size_t index);
    static std::filesystem::path SafeExportName(const std::string& name);
    void UnloadFiles();
    void LoadImageFile(const std::string& path);
    void LoadAudioFile(const std::string& path);
    void LoadModelFile(const std::string& path);
    void ResetContainer();
    bool HaveLoaded() const;
    uint64_t CapacityBytes() const;
};
