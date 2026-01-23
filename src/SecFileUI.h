#pragma once
#include "raylib.h"
#include "SecFile.h"

class SecFileUI
{
    SecFile* secFile;
    Vector2 screenSize;
    Texture2D* dropTexture;
    
public:
    SecFileUI(SecFile* secFile,Vector2 screenSize);
    
    void Draw();
    void DrawImageDropZone();
    Texture2D* GetDropTexture();
    bool IsFileDroppedInRect(const Rectangle& rect, std::string& path);
};
