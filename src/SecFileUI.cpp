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
#include "raymath.h"
#include "rlgl.h"

namespace fs = std::filesystem;

SecFileUI::SecFileUI(SecFile* secFile, Vector2 screenSize)
{
    this->secFile = secFile;
    this->screenSize = screenSize;

    camera = Camera3D();
    camera.position = Vector3(0, 1, -5);
    camera.target = Vector3(0, 0, 0);
    camera.up = Vector3(0, 1, 0);
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
}

void SecFileUI::Draw()
{
    DrawDropZone();
    if (!secFile->HaveLoaded())return;
    
    DrawMenu();
    if (secFile->container != nullptr)
    {
        DrawContainer();   
    }
}

void SecFileUI::DrawDropZone()
{
    Rectangle rect {0,0, screenSize.x,screenSize.y};
    if (secFile->container != nullptr)
    {
        rect.width -= 320 + padding;
        rect.x = padding;
    }

    Vector2 center{screenSize.x/2.f, screenSize.y/2.f};
    float extend = 50;
    DrawTextEx(*font,"Drop file here! (.jpg, .png, .wav)",{center.x-200, center.y-100},32, 1, WHITE);
        
    Texture2D tex = *GetDropTexture();
    Rectangle dest{center.x - extend, center.y - extend, extend*2, extend*2};
    DrawTexturePro(tex, {0.f,0.f, (float)tex.width, (float)tex.height}, dest, {0.f,0.f}, 0.f, WHITE);
    
    std::string filePath;
    if (IsFileDroppedInRect(rect, filePath))
    {
        std::cout << "Load file " << filePath << std::endl;

        fs::path path(filePath);
        std::string ext = path.extension().string();

        if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
        {
            UnloadPreviews();
            secFile->LoadImageFile(filePath);
            UpdatePreview();
        }else

        if (ext == ".wav" || ext == ".mp3")
        {
            UnloadPreviews();
            secFile->LoadAudioFile(filePath);
            UpdatePreview();
        }else
        
        if (ext == ".obj")
        {
            UnloadPreviews();
            secFile->LoadModelFile(filePath);
            UpdatePreview();
        }

        else
        {
            TraceLog(LOG_WARNING, "Unsupported file extension %s", ext.c_str());
        }
    }

    if (previewTexture != nullptr)
    {
        DrawPreviewTexture(rect);
    }

    if (previewSound != nullptr)
    {
        DrawPreviewAudio(rect);
    }

    if (previewModel != nullptr)
    {
        DrawPreviewModel(rect);
    }
}

void SecFileUI::DrawPreviewTexture(Rectangle rect)
{
    rect = FitSizeInto(previewTexture->width, previewTexture->height, rect);
    Rectangle source {0.f,0.f, (float)previewTexture->width, (float)previewTexture->height};
    Texture2D tex = *previewTexture;
    DrawTexturePro(tex, source, rect, {0.,0.}, 0., WHITE);
}

void SecFileUI::DrawPreviewAudio(Rectangle rect)
{
    Vector2 viewSize = {rect.width, 300};
    Rectangle viewRect = FitSizeInto(viewSize.x, viewSize.y, rect);

    Color bgColor= {54,101,107, 255};
    Color midLineColor {71,112, 132, 255};
    Color waveColor {242,78,70,255};

    float midHeight = viewRect.y + viewRect.height / 2;
    DrawRectangle(viewRect.x, viewRect.y, viewRect.width, viewRect.height, bgColor);
    DrawLine(viewRect.x, midHeight, viewRect.x + viewRect.width, midHeight, midLineColor);

    if (previewWaveData.size() != viewRect.width)
    {
        previewWaveData = ExtractSizedSamples(previewWave, viewRect.width);
    }
    
    for (int i=0; i < previewWaveData.size(); i++)
    {
        Vector2 start{viewRect.x + i, midHeight};
        Vector2 end{start.x, midHeight + (viewRect.height / 2 * previewWaveData[i])};
        DrawLine(start.x, start.y, end.x, end.y, waveColor);
    }

    long duration = secFile->loadedWave->frameCount / secFile->loadedWave->sampleRate * 1000;
    long elapsed = isAudioPlaying ? std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - audioStartPlaying).count() : 0;

    std::stringstream timeText;
    timeText << (elapsed / 60000) << ":" << std::setw(2) << std::setfill('0') << (elapsed / 1000 % 60);
    timeText << " / ";
    timeText << (duration / 60000) << ":" << std::setw(2) << std::setfill('0') << (duration / 1000 % 60);
    DrawTextEx(
        *font,
        timeText.str().c_str(),
        {viewRect.x + padding*2 + 100, viewRect.y + viewRect.height - padding - 12},
        14,
        1,
        WHITE);

    if (previewSound != nullptr && IsSoundPlaying(*previewSound))
    {
        int playPos = viewRect.width * (elapsed / (float)duration);
        DrawLine(viewRect.x + playPos, viewRect.y, viewRect.x + playPos, viewRect.y + viewRect.height, WHITE);
    }

    Rectangle buttonRect {
        viewRect.x + padding,
    (viewRect.y + viewRect.height) - (padding + 25),
    100,
    25};

    if (isAudioPlaying)
    {
        if (GuiButton(buttonRect,"Stop"))
        {
            isAudioPlaying = false;
        }
    }else
    {
        if (GuiButton(buttonRect,"Play"))
        {
            isAudioPlaying = true;
        }
    }

    if (isAudioPlaying != IsSoundPlaying(*previewSound))
    {
        if (isAudioPlaying)
        {
            PlaySound(*previewSound);
            audioStartPlaying = clock::now();
        }else
        {
            StopSound(*previewSound);
        }
    }
}

void SecFileUI::DrawPreviewModel(Rectangle rect)
{
    if (meshTexture  == nullptr)return;
    
    auto src = Rectangle{0,0,(float)meshTexture->texture.width, -(float)meshTexture->texture.height};
    DrawTexturePro(meshTexture->texture, src, rect, {0,0}, 0, WHITE);
}

void SecFileUI::UpdateModelTexture()
{
    if (meshTexture == nullptr)
    {
        meshTexture = new RenderTexture2D(LoadRenderTexture(1920, 1080));
    }
    
    BeginTextureMode(*meshTexture);
    ClearBackground(DARKGRAY);
    BeginMode3D(camera);

    auto now = clock::now().time_since_epoch();
    long ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    long rotTimeMs = 10000;
    ms %= rotTimeMs;
        
    float rotation = (ms / (float)rotTimeMs)*360.;
    DrawModelEx(*previewModel, {0,0,0},{0.,1.,0.}, rotation, {1.,1.,1.}, WHITE);
    
    EndMode3D();
    EndTextureMode();
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
    if (secFile->HaveLoaded())DrawSize();
}

void SecFileUI::DrawBitSlider()
{
    Rectangle rect{padding + space + 150, screenSize.y - padding - 25, 150, 25};
    std::string label = "BitDepth";

    std::stringstream value;
    int bitDepth = (int)secFile->bitDepth;
    value << bitDepth;
    float sliderVal = bitDepth;
    float maxValue = -1;
    if (secFile->loadedTexture != nullptr)
    {
        maxValue = 8.f;
    }else if (secFile->loadedWave != nullptr)
    {
        maxValue = 16.f;
    }else if (secFile->loadedModel != nullptr)
    {
        maxValue = 32.f;
    }
    else
    {
        throw new std::exception("Something should be loaded!");
    }
    GuiSlider(rect, label.c_str(), value.str().c_str(), &sliderVal, 0.f, maxValue);
    if (bitDepth != std::round(sliderVal))
    {
        bitDepth = std::round(sliderVal);
        secFile->bitDepth = bitDepth;
        UpdatePreview();
    }
}

void SecFileUI::UpdatePreview()
{
    std::cout << "Update Preview to "<< (int)secFile->bitDepth << std::endl;
    
    if (secFile->loadedTexture != nullptr)UpdateImagePreview();
    if (secFile->loadedWave != nullptr)UpdateAudioPreview();
    if (secFile->loadedModel != nullptr)UpdateModelPreview();
}

void SecFileUI::UpdateImagePreview()
{
    if (previewTexture != nullptr)UnloadTexture(*previewTexture);
    Image img = LoadImageFromTexture(*secFile->loadedTexture);
    if (secFile->bitDepth > 0)
    {
        auto colors = SecFile::ImageToByteVector(img);
        uint8_t mask = BitStream::GetMask(0, secFile->bitDepth - 1);
        for (int c=0; c<colors.size(); c++)
        {
            colors[c] &= ~mask;
            colors[c] |= (Random::GetByte() & mask);
        }
        std::memcpy(img.data, colors.data(), colors.size());
    }
    
    previewTexture = new Texture2D(LoadTextureFromImage(img));
    UnloadImage(img);
}

void SecFileUI::UpdateAudioPreview()
{
    UnloadPreviews();
    
    previewWave = new Wave(*secFile->loadedWave);
    long dataSize = previewWave->frameCount * previewWave->channels * (previewWave->sampleSize / 8);
    previewWave->data = MemAlloc(dataSize);
    memcpy(previewWave->data, secFile->loadedWave->data, dataSize);
    
    if (secFile->bitDepth == 0)
    {
        previewSound = new Sound(LoadSoundFromWave(*secFile->loadedWave));
        previewWaveData.clear();
        return;
    }
    
    if (previewWave->sampleSize != 16 || previewWave->channels != 1)
    {
        WaveFormat(previewWave, (int)previewWave->sampleRate, 16, 1);
    }
    
    uint8_t* dataPtr = (uint8_t*)previewWave->data;
    std::vector<uint8_t> data(dataPtr, dataPtr + previewWave->frameCount * 2);
    auto bitDepths = BitStream::GetByteLevelBitDepths(secFile->bitDepth, 2);
    for (int i = 0; i < data.size(); i++)
    {
        uint8_t mask = BitStream::GetMask(0, bitDepths[i%bitDepths.size()]-1);
        data[i] &= ~mask;
        data[i] |= (Random::GetByte() & mask);
    }
    std::memcpy(previewWave->data, data.data(), data.size());
    
    previewSound = new Sound(LoadSoundFromWave(*previewWave));
    previewWaveData.clear();
}

void SecFileUI::UpdateModelPreview()
{
    previewModel = secFile->loadedModel;
    previewModel->materials[0].shader = *shader;
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
        secFile->SaveLoaded();
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
    if (!secFile->HaveLoaded())return;
    
    Vector2 pos = {padding + 400, screenSize.y - padding - 20};

    long mediaSize = 0;

    if (secFile->loadedTexture != nullptr)
    {
        long byteCount = ByteCount(*secFile->loadedTexture);
        mediaSize = SecFile::GetSizeInBytes(byteCount, 8, secFile->bitDepth);
    }

    if (secFile->loadedWave != nullptr)
    {
        long byteCount = ByteCount(*secFile->loadedWave);
        mediaSize = SecFile::GetSizeInBytes(byteCount, secFile->loadedWave->sampleSize, secFile->bitDepth);
    }

    long contentSize = contentSize = secFile->container == nullptr ? 0 : secFile->container->GetSize() + KEY.size();
    
    std::stringstream ss;
    ss << contentSize << "/" << mediaSize;
    DrawTextEx(*font, ss.str().c_str(), {pos.x, pos.y}, 14, 1, contentSize > mediaSize ? RED : GREEN);
}

void SecFileUI::SetFont(Font* font)
{
    this->font = font;
}

void SecFileUI::SetShader(Shader* shader)
{
    this->shader = shader;
}

std::vector<float> SecFileUI::ExtractSizedSamples(Wave* wave, const int sampleCount)
{
    if (wave->channels != 1 || wave->sampleSize != 16)
    {
        WaveFormat(wave, wave->sampleRate, 16, 1);
    }

    int16_t* data = (int16_t*)wave->data;
    std::vector<int16_t> samples(data, data + wave->frameCount);
    std::vector<float> results(sampleCount);
    int step = samples.size()/results.size();

    for (int i=0; i<results.size(); i++)
    {
        int start = i*step;
        int end = std::min(start+step, (int)samples.size());
        int range = end - start;
        int avg = 0;
        for (int s=start; s<end; s++)
        {
            avg += samples[s] / range;
        }
        results[i] = (float)avg / std::numeric_limits<int16_t>::max();
    }

    float highest = 0.f;
    for (auto v : results)
    {
        if (std::abs(v) > highest)highest = std::abs(v);
    }

    for (int i=0; i<results.size(); i++)
    {
        results[i] /= highest;
    }

    return results;
}

void SecFileUI::UnloadPreviews()
{
    if (previewTexture != nullptr)UnloadTexture(*previewTexture);
    delete previewTexture;
    previewTexture = nullptr;

    isAudioPlaying = false;
    if (previewSound != nullptr)
    {
        if (IsSoundPlaying(*previewSound))StopSound(*previewSound);
        UnloadSound(*previewSound);
        delete previewSound;
        previewSound = nullptr;
    }
    previewWaveData.clear();

    if (previewWave != nullptr)UnloadWave(*previewWave);
    previewWave = nullptr;
}
