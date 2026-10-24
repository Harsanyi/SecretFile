#pragma once
#include "SecFile.h"
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

class SecFileUI
{
    const Color BG_COLOR = {44,44,44,255};

    Font* font = nullptr;
    SecFile* secFile;
    Vector2 screenSize;
    float padding = 10;
    float space = 5;
    Texture2D* previewTexture = nullptr;
    Sound* previewSound = nullptr;
    Wave* previewWave = nullptr;
    Model* previewModel = nullptr;
    std::vector<float> previewWaveData;
    bool isAudioPlaying = false;
    std::size_t firstVisibleFile = 0;
    using clock = std::chrono::steady_clock;
    clock::time_point audioStartPlaying;
    Camera3D camera {0};
    Shader* shader = nullptr;
    RenderTexture2D* meshTexture = nullptr;

public:
    SecFileUI(SecFile* secFile,Vector2 screenSize);
    ~SecFileUI();

    void Draw();
    void DrawDropZone();
    void DrawPreviewTexture(Rectangle rect);
    void DrawPreviewAudio(Rectangle rect);
    void DrawPreviewModel(Rectangle rect);
    void UpdateModelTexture();
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
    static std::vector<float> ExtractSizedSamples(const Wave* wave, int sampleCount);
    void UnloadPreviews();
};
