#pragma once
#include "SecFileUI.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Extensions.h"
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
    if (secFile->loadedTexture == nullptr)return;
    
    DrawMenu();
    if (secFile->container != nullptr)
    {
        DrawContainer();   
    }
}

void SecFileUI::DrawImageDropZone()
{
    Rectangle rect {0,0, screenSize.x,screenSize.y};
    if (secFile->container != nullptr)
    {
        rect.width -= 320 + padding;
        rect.x = padding;
    }
    
    if (previewTexture != nullptr)
    {
        rect = FitSizeInto(previewTexture->width, previewTexture->height, rect);
        Rectangle source {0.f,0.f, (float)previewTexture->width, (float)previewTexture->height};
        Texture2D tex = *previewTexture;
        DrawTexturePro(tex, source, rect, {0.,0.}, 0., WHITE);
    }else
    {
        Vector2 center{screenSize.x/2.f, screenSize.y/2.f};
        float extend = 50;
        DrawTextEx(*font,"Drop file here! (.jpg, .png)",{center.x-150, center.y-100},32, 1, WHITE);
        
        Texture2D tex = *GetDropTexture();
        Rectangle dest{center.x - extend, center.y - extend, extend*2, extend*2};
        DrawTexturePro(tex, {0.f,0.f, (float)tex.width, (float)tex.height}, dest, {0.f,0.f}, 0.f, WHITE);
    }

    std::string filePath;
    if (IsFileDroppedInRect(rect, filePath))
    {
        std::cout << "Load file " << filePath << std::endl;

        fs::path path(filePath);
        std::string ext = path.extension().string();
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")return;
        
        if (secFile->loadedTexture != nullptr)UnloadTexture(*secFile->loadedTexture);
        secFile->loadedTexture = new Texture2D(LoadTexture(filePath.c_str()));
        secFile->loadedTextureName = path.filename().string(); 
        UpdatePreview();
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

void SecFileUI::DrawMenu()
{
    if (secFile->container == nullptr)
    {
        if (secFile->bitDepth < 1)
        {
            GuiDisable();
            DrawOpenButton();
            GuiEnable();
        }else
        {
            DrawOpenButton();
        }
    }else
    {
        DrawSaveButton();
    }
        
    DrawBitSlider();
    if (secFile->loadedTexture != nullptr)DrawSize();
}

void SecFileUI::DrawBitSlider()
{
    Rectangle rect{padding + space + 150, screenSize.y - padding - 25, 150, 25};
    std::string label = "BitDepth";

    std::stringstream value;
    int bitDepth = (int)secFile->bitDepth;
    value << bitDepth;
    float sliderVal = bitDepth;
    GuiSlider(rect, label.c_str(), value.str().c_str(), &sliderVal, 0.f, 8.f);
    if (bitDepth != std::round(sliderVal))
    {
        bitDepth = std::round(sliderVal);
        secFile->bitDepth = bitDepth;
        UpdatePreview();
    }
}

void SecFileUI::UpdatePreview()
{
    std::cout << "Update Preview" << std::endl;
    
    if (secFile->loadedTexture == nullptr)return;
    if (previewTexture != nullptr)UnloadTexture(*previewTexture);
    
    Image img = LoadImageFromTexture(*secFile->loadedTexture);
    if (secFile->bitDepth > 0)
    {
        auto colors = SecFile::ImageToByteVector(img);
        BitStream bs(colors, secFile->bitDepth);
        long dataSize = SecFile::GetSizeInBytes(img.width, img.height, secFile->bitDepth);
        std::vector<uint8_t> data = Random::GetBytes(dataSize);
        bs.WriteBytes(data);
        std::memcpy(img.data, bs.data.data(), dataSize);
    }
    
    previewTexture = new Texture2D(LoadTextureFromImage(img));
    UnloadImage(img);
}

void SecFileUI::DrawOpenButton()
{
    Rectangle rect{padding, screenSize.y - padding - 25, 100, 25};
    if (GuiButton(rect,"Open") == 1)
    {
        std::cout << "Open" << std::endl;
        if (secFile->bitDepth > 0)
        {
            secFile->LoadContainer();
        }
    }
}

void SecFileUI::DrawSaveButton()
{
    Rectangle rect{padding, screenSize.y - padding - 25, 100, 25};
    if (GuiButton(rect,"Save") == 1)
    {
        std::cout << "Save" << std::endl;
        secFile->SaveImage();
    }
}

void SecFileUI::DrawContainer()
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
        Vector2 pos {rect.x + padding, rect.y + padding + f*size.y + (f > 0 ? (f-1)*space : 0)};
        DrawTextEx(*font,files[f].fileName.c_str(), {pos.x, pos.y}, 14, 1, WHITE);

        //del
        Rectangle delRect{rect.x + 200, pos.y, 40, 25};
        if (GuiButton(delRect, "Del")==1)
        {
            secFile->RemoveFileAt(f);
            return;
        }

        //Save
        Rectangle saveRect{delRect.x + delRect.width + space, delRect.y, 40, 25};
        if (GuiButton(saveRect, "Exp") == 1)
        {
            secFile->ExportFileAt(f);
        }
    }
}

void SecFileUI::DrawSize()
{
    if (secFile->loadedTexture == nullptr)return;
    
    Vector2 pos = {padding + 400, screenSize.y - padding - 20};
    auto imgSize = SecFile::GetSizeInBytes(secFile->loadedTexture->width, secFile->loadedTexture->height, secFile->bitDepth);
    auto contentSize = secFile->container == nullptr ? 0 : secFile->container->GetSize() + KEY.size();
    std::stringstream ss;
    ss << contentSize << "/" << imgSize;
    DrawTextEx(*font, ss.str().c_str(), {pos.x, pos.y}, 14, 1, contentSize > imgSize ? RED : GREEN);
}

void SecFileUI::SetFont(Font* font)
{
    this->font = font;
}
