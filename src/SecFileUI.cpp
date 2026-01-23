#pragma once

#include "SecFileUI.h"

#include <iostream>
#include "raygui.h"
#include "SecFile.h"

SecFileUI::SecFileUI(SecFile* secFile, Vector2 screenSize)
{
    this->secFile = secFile;
    this->screenSize = screenSize;
}

void SecFileUI::Draw()
{
    DrawImageDropZone();
    DrawTestButton();
    DrawMenu();
}

void SecFileUI::DrawImageDropZone()
{
    Rectangle rect{0.f,0.f, screenSize.x, screenSize.y};
    std::string filePath;
    if (IsFileDroppedInRect(rect, filePath))
    {
        std::cout << "Load file " << filePath << std::endl;
        if (secFile->loadedTexture != nullptr)UnloadTexture(*secFile->loadedTexture);
        secFile->loadedTexture = new Texture2D(LoadTexture(filePath.c_str()));
    }

    if (secFile->loadedTexture != nullptr)
    {
        Rectangle source {0.f,0.f, (float)secFile->loadedTexture->width, (float)secFile->loadedTexture->height};
        Texture2D tex = *secFile->loadedTexture;
        DrawTexturePro(tex, source, rect, {0.,0.}, 0., WHITE);
    }else
    {
        Texture2D tex = *GetDropTexture();
        Vector2 center{screenSize.x/2.f, screenSize.y/2.f};
        float extend = 50;
        Rectangle dest{center.x - extend, center.y - extend, extend*2, extend*2};
        DrawTexturePro(tex, {0.f,0.f, (float)tex.width, (float)tex.height}, dest, {0.f,0.f}, 0.f, WHITE);
    }
}

Texture2D* SecFileUI::GetDropTexture()
{
    if (dropTexture == nullptr)
    {
        dropTexture = new Texture2D(LoadTexture("drop-icon.png"));
    }
    return dropTexture;
}

bool SecFileUI::IsFileDroppedInRect(const Rectangle& rect, std::string& path)
{
    if (IsFileDropped())
    {
        auto files = LoadDroppedFiles();
        if (CheckCollisionPointRec(GetMousePosition(), rect))
        {
            path = std::string(files.paths[0]);
            UnloadDroppedFiles(files);
            return true;
        }
    }

    return false;
}

void SecFileUI::DrawTestButton()
{
    Vector2 size {100,25};
    Rectangle rect {screenSize.x - size.x - padding, padding, size.x, size.y};
    
    if (GuiButton(rect, "Test") == 1)
    {
        std::cout << "Test" << std::endl;
    }
}

void SecFileUI::DrawMenu()
{
    
}
