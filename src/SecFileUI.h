#pragma once
#include "SecFile.h"

class SecFileUI
{
    SecFile* secFile;
    Vector2 screenSize;
    Texture2D* dropTexture;
    float padding = 10;
    float space = 5;
    
public:
    SecFileUI(SecFile* secFile,Vector2 screenSize);
    
    void Draw();
    void DrawImageDropZone();
    Texture2D* GetDropTexture();
    bool IsFileDroppedInRect(const Rectangle& rect, std::string& path);
    void DrawTestButton();
    void DrawMenu();
};
