#pragma once
#include "SecFile.h"
#include <filesystem>

namespace fs = std::filesystem;

class SecFileUI
{
    const Color BG_COLOR = {44,44,44,255};
    
    Font* font;
    SecFile* secFile;
    Vector2 screenSize;
    Texture2D* dropTexture;
    float padding = 10;
    float space = 5;
    Texture2D* previewTexture = nullptr;
    Sound* previewSound = nullptr;
    Wave* previewWave = nullptr;
    Model* previewModel = nullptr;
    std::vector<float> previewWaveData;
    bool isAudioPlaying = false;
    using clock = std::chrono::steady_clock;
    clock::time_point audioStartPlaying;
    Camera3D camera {0};
    Shader* shader;
    RenderTexture2D* meshTexture = nullptr;
        
public:
    SecFileUI(SecFile* secFile,Vector2 screenSize);
    
    void Draw();
    void DrawDropZone();
    void DrawPreviewTexture(Rectangle rect);
    void DrawPreviewAudio(Rectangle rect);
    void DrawPreviewModel(Rectangle rect);
    void UpdateModelTexture();
    Texture2D* GetDropTexture();
    bool IsFileDroppedInRect(const Rectangle& rect, std::string& path);
    void DrawMenu();
    void DrawBitSlider();
    void UpdatePreview();
    void UpdateImagePreview();
    void UpdateAudioPreview();
    void UpdateModelPreview();
    void DrawOpenButton();
    void DrawSaveButton();
    void DrawContainer();
    void DrawSize();
    void SetFont(Font* font);
    void SetShader(Shader* shader);
    std::vector<float> ExtractSizedSamples(Wave* wave, const int sampleCount);
    void UnloadPreviews();
};
