#pragma once
#include "SecFile.h"
#include <filesystem>

namespace fs = std::filesystem;

class SecFileUI
{
    Font* font;
    SecFile* secFile;
    Vector2 screenSize;
    Texture2D* dropTexture;
    float padding = 10;
    float space = 5;
    Texture2D* previewTexture = nullptr;
    Sound* previewSound = nullptr;
    Wave* previewWave = nullptr;
    std::vector<float> previewWaveData;
    bool isAudioPlaying = false;
    using clock = std::chrono::steady_clock;
    clock::time_point audioStartPlaying;
        
public:
    SecFileUI(SecFile* secFile,Vector2 screenSize);
    
    void Draw();
    void DrawDropZone();
    void DrawPreviewTexture(Rectangle rect);
    void DrawPreviewAudio(Rectangle rect);
    Texture2D* GetDropTexture();
    bool IsFileDroppedInRect(const Rectangle& rect, std::string& path);
    void DrawMenu();
    void DrawBitSlider();
    void UpdatePreview();
    void UpdateImagePreview();
    void UpdateAudioPreview();
    void DrawOpenButton();
    void DrawSaveButton();
    void DrawContainer();
    void DrawSize();
    void SetFont(Font* font);
    std::vector<float> ExtractSizedSamples(Wave* wave, const int sampleCount);
    void UnloadPreviews();
};
