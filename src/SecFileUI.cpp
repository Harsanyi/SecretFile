#pragma once

#include "SecFileUI.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Random.h"
#include "raygui.h"
#include "SecFile.h"
#include "filesystem"

namespace fs = std::filesystem;

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
    if (secFile->container != nullptr)
    {
        DrawContent();   
    }
}

void SecFileUI::DrawImageDropZone()
{
    Rectangle rect{0.f,0.f, screenSize.x-350, screenSize.y};
    std::string filePath;
    if (IsFileDroppedInRect(rect, filePath))
    {
        std::cout << "Load file " << filePath << std::endl;
        if (secFile->loadedTexture != nullptr)UnloadTexture(*secFile->loadedTexture);
        secFile->loadedTexture = new Texture2D(LoadTexture(filePath.c_str()));
        UpdatePreview();
    }

    if (previewTexture != nullptr)
    {
        Rectangle source {0.f,0.f, (float)previewTexture->width, (float)previewTexture->height};
        Texture2D tex = *previewTexture;
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
    Vector2 size{160, 100};
    Rectangle rect{padding, screenSize.y - padding - size.y, size.x, size.y};
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, DARKBROWN);
    
    DrawBitSlider();
    DrawOpenButton();
}

void SecFileUI::DrawBitSlider()
{
    Rectangle rect{padding + 90, screenSize.y - 70, 150, 25};
    std::string label = "Bit Slider";
    std::stringstream value;
    value << secFile->bitDepth;
    float bitDepth = secFile->bitDepth;
    GuiSlider(rect, label.c_str(), value.str().c_str(), &bitDepth, 0.f, 8.f);
    if (secFile->bitDepth != std::round(bitDepth))
    {
        secFile->bitDepth = std::round(bitDepth);
        UpdatePreview();
    }
}

void SecFileUI::UpdatePreview()
{
    std::cout << "Update Preview" << std::endl;
    if (secFile->loadedTexture == nullptr)return;
    if (previewTexture != nullptr)
    {
        UnloadTexture(*previewTexture);
    }

    Image img = LoadImageFromTexture(*secFile->loadedTexture);
    previewTexture = new Texture2D(LoadTextureFromImage(img));
    UnloadImage(img);
    if (secFile->bitDepth < 1)return;
    
    long dataSize = SecFile::GetSizeInBytes(previewTexture, secFile->bitDepth);
    dataSize -= 100;
    std::vector<uint8_t> data = Random::GetBytes(dataSize);
    SecFile::WriteDataToTexture(*previewTexture, data, secFile->bitDepth);
}

void SecFileUI::DrawOpenButton()
{
    Rectangle rect{padding, screenSize.y - 100, 100, 25};
    if (GuiButton(rect,"Open") == 1)
    {
        std::cout << "Open" << std::endl;
        secFile->LoadContainer();
    }
}

void SecFileUI::DrawContent()
{
    Vector2 size {300,screenSize.y - padding*2};
    Rectangle rect{screenSize.x - padding - size.x, screenSize.y - padding - size.y, size.x, size.y};
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, DARKBROWN);

    if (secFile->container == nullptr)return;

    std::string path;
    if (IsFileDroppedInRect(rect, path))
    {
       secFile->AddFileFromPath(path);
    }

    auto files = secFile->container->files;
    for (int f=0; f<files.size(); f++)
    {
        //text
        Vector2 size{150,25};
        Rectangle fRect{rect.x + padding, rect.y + padding + f*size.y + (f > 0 ? (f-1)*space : 0), size.x, size.y};
        DrawText(files[f].fileName.c_str(), fRect.x, fRect.y, 14, WHITE);

        //del
        Rectangle delRect{fRect.x+150, fRect.y, 25, 25};
        if (GuiButton(delRect, "X")==1)
        {
            secFile->RemoveFileAt(f);
            return;
        }

        //Save
        Rectangle saveRect{delRect.x + delRect.width + space, delRect.y, 25, 25};
        if (GuiButton(saveRect, "S") == 1)
        {
            secFile->ExportFileAt(f);
        }
    }
}
