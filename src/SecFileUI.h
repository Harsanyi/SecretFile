#pragma once
#include "SecFile.h"

class SecFileUI
{
    Font* font;
    SecFile* secFile;
    Vector2 screenSize;
    Texture2D* dropTexture;
    float padding = 10;
    float space = 5;
    Texture2D* previewTexture = nullptr;
    
public:
    SecFileUI(SecFile* secFile,Vector2 screenSize);
    
    void Draw();
    void DrawImageDropZone();
    Texture2D* GetDropTexture();
    bool IsFileDroppedInRect(const Rectangle& rect, std::string& path);
    void DrawMenu();
    void DrawBitSlider();
    void UpdatePreview();
    void DrawOpenButton();
    void DrawSaveButton();
    void DrawContent();
    void SetFont(Font* font);
};
