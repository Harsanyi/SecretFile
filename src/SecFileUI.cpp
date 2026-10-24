#include "SecFileUI.h"
#include "Extensions.h"
#include "Random.h"
#include "raygui.h"
#include "raymath.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>

SecFileUI::SecFileUI(SecFile* file, Vector2 size) : secFile(file), screenSize(size)
{
    camera.position = {0, 1, -5};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
}

SecFileUI::~SecFileUI() { UnloadPreviews(); }

void SecFileUI::Draw()
{
    try
    {
        if (IsFileDropped())
        {
            const auto dropped = LoadDroppedFiles();
            try
            {
                const Rectangle panel{screenSize.x - padding - 300, padding, 300, screenSize.y - padding * 2};
                for (unsigned i = 0; i < dropped.count; ++i)
                {
                    const std::string path = dropped.paths[i];
                    if (secFile->container && CheckCollisionPointRec(GetMousePosition(), panel))
                        secFile->AddFileFromPath(path);
                    else
                    {
                        std::string extension = fs::path(path).extension().string();
                        std::transform(extension.begin(), extension.end(), extension.begin(),
                                       [](unsigned char c) { return char(std::tolower(c)); });
                        if (extension != ".jpg" && extension != ".jpeg" && extension != ".png" &&
                            extension != ".wav" && extension != ".obj")
                            throw std::runtime_error("Supported carriers: .jpg, .jpeg, .png, .wav, .obj");
                        UnloadPreviews();
                        if (extension == ".wav") secFile->LoadAudioFile(path);
                        else if (extension == ".obj") secFile->LoadModelFile(path);
                        else secFile->LoadImageFile(path);
                        UpdatePreview();
                    }
                }
            }
            catch (...) { UnloadDroppedFiles(dropped); throw; }
            UnloadDroppedFiles(dropped);
        }
        DrawDropZone();
        if (secFile->HaveLoaded())
        {
            DrawMenu();
            if (secFile->container) DrawContainer();
        }
    }
    catch (const std::exception& error)
    {
        secFile->status = error.what();
        TraceLog(LOG_WARNING, "%s", error.what());
    }
    if (font && !secFile->status.empty())
        DrawTextEx(*font, secFile->status.c_str(), {padding, screenSize.y - 55}, 14, 1, LIGHTGRAY);
}

void SecFileUI::DrawDropZone()
{
    Rectangle rect{padding, padding, screenSize.x - padding * 2, screenSize.y - 80};
    if (secFile->container) rect.width -= 320;
    if (!secFile->HaveLoaded())
    {
        DrawTextEx(*font, "Drop a carrier here", {screenSize.x / 2 - 180, screenSize.y / 2 - 60}, 32, 1, WHITE);
        DrawTextEx(*font, ".jpg  .jpeg  .png  .wav  .obj", {screenSize.x / 2 - 180, screenSize.y / 2}, 20, 1, LIGHTGRAY);
    }
    if (previewTexture) DrawPreviewTexture(rect);
    if (previewSound) DrawPreviewAudio(rect);
    if (previewModel) DrawPreviewModel(rect);
}

void SecFileUI::DrawPreviewTexture(Rectangle rect)
{
    rect = FitSizeInto(previewTexture->width, previewTexture->height, rect);
    DrawTexturePro(*previewTexture, {0, 0, float(previewTexture->width), float(previewTexture->height)},
                   rect, {0, 0}, 0, WHITE);
}

void SecFileUI::DrawPreviewAudio(Rectangle rect)
{
    const auto view = FitSizeInto(int(rect.width), 300, rect);
    const int columns = std::max(1, int(view.width));
    if (previewWaveData.size() != std::size_t(columns)) previewWaveData = ExtractSizedSamples(previewWave, columns);
    const float middle = view.y + view.height / 2;
    DrawRectangleRec(view, {54, 101, 107, 255});
    DrawLine(int(view.x), int(middle), int(view.x + view.width), int(middle), {71, 112, 132, 255});
    for (std::size_t i = 0; i < previewWaveData.size(); ++i)
        DrawLine(int(view.x + i), int(middle), int(view.x + i),
                 int(middle + view.height / 2 * previewWaveData[i]), {242, 78, 70, 255});

    const uint64_t duration = previewWave->sampleRate ? uint64_t(previewWave->frameCount) * 1000 / previewWave->sampleRate : 0;
    if (isAudioPlaying && !IsSoundPlaying(*previewSound)) isAudioPlaying = false;
    const uint64_t elapsed = isAudioPlaying ? std::min<uint64_t>(duration,
        std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - audioStartPlaying).count()) : 0;
    std::ostringstream text;
    text << elapsed / 60000 << ':' << std::setw(2) << std::setfill('0') << elapsed / 1000 % 60
         << " / " << duration / 60000 << ':' << std::setw(2) << duration / 1000 % 60;
    DrawTextEx(*font, text.str().c_str(), {view.x + padding * 2 + 100, view.y + view.height - padding - 12}, 14, 1, WHITE);
    if (isAudioPlaying && duration)
    {
        const float x = view.x + view.width * float(double(elapsed) / duration);
        DrawLine(int(x), int(view.y), int(x), int(view.y + view.height), WHITE);
    }
    if (!duration) GuiDisable();
    if (GuiButton({view.x + padding, view.y + view.height - padding - 25, 100, 25}, isAudioPlaying ? "Stop" : "Play"))
    {
        if (isAudioPlaying) StopSound(*previewSound);
        else { PlaySound(*previewSound); audioStartPlaying = clock::now(); }
        isAudioPlaying = !isAudioPlaying;
    }
    GuiEnable();
}

void SecFileUI::DrawPreviewModel(Rectangle rect)
{
    if (!meshTexture) return;
    rect = FitSizeInto(meshTexture->texture.width, meshTexture->texture.height, rect);
    DrawTexturePro(meshTexture->texture, {0, 0, float(meshTexture->texture.width), -float(meshTexture->texture.height)},
                   rect, {0, 0}, 0, WHITE);
}

void SecFileUI::UpdateModelTexture()
{
    if (!previewModel) return;
    if (!meshTexture) meshTexture = new RenderTexture2D(LoadRenderTexture(1920, 1080));
    BeginTextureMode(*meshTexture);
    ClearBackground(DARKGRAY);
    BeginMode3D(camera);
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now().time_since_epoch()).count();
    const float angle = float(milliseconds % 10000) / 10000 * 360;
    DrawModelEx(*previewModel, {0, 0, 0}, {0, 1, 0}, angle, {1, 1, 1}, WHITE);
    EndMode3D();
    EndTextureMode();
}

void SecFileUI::DrawMenu()
{
    if (secFile->bitDepth == 0) GuiDisable();
    if (secFile->container) DrawSaveButton();
    else DrawOpenButton();
    GuiEnable();
    DrawBitSlider();
    DrawSize();
}

void SecFileUI::DrawBitSlider()
{
    const int maximum = secFile->loadedTexture ? 8 : secFile->loadedWave ? 16 : 32;
    float value = secFile->bitDepth;
    const auto label = std::to_string(secFile->bitDepth);
    GuiSlider({padding + space + 150, screenSize.y - padding - 25, 150, 25}, "BitDepth",
              label.c_str(), &value, 0, float(maximum));
    const auto selected = static_cast<uint8_t>(std::round(value));
    if (selected != secFile->bitDepth)
    {
        secFile->bitDepth = selected;
        UpdatePreview();
    }
}

void SecFileUI::UpdatePreview()
{
    UnloadPreviews();
    try
    {
        if (secFile->loadedTexture) UpdateImagePreview();
        else if (secFile->loadedWave) UpdateAudioPreview();
        else if (secFile->loadedModel) UpdateModelPreview();
    }
    catch (...) { UnloadPreviews(); throw; }
}

void SecFileUI::UpdateImagePreview()
{
    Image image = LoadImageFromTexture(*secFile->loadedTexture);
    try
    {
        auto bytes = SecFile::ImageToByteVector(image);
        if (secFile->bitDepth)
        {
            const auto mask = BitStream::GetMask(0, secFile->bitDepth - 1);
            for (auto& byte : bytes) byte = uint8_t((byte & ~mask) | (Random::GetByte() & mask));
            std::memcpy(image.data, bytes.data(), bytes.size());
        }
        previewTexture = new Texture2D(LoadTextureFromImage(image));
    }
    catch (...) { UnloadImage(image); throw; }
    UnloadImage(image);
}

void SecFileUI::UpdateAudioPreview()
{
    previewWave = new Wave(WaveCopy(*secFile->loadedWave));
    if (secFile->bitDepth)
    {
        const auto pattern = BitStream::GetByteLevelBitDepths(secFile->bitDepth, 2);
        auto* bytes = static_cast<uint8_t*>(previewWave->data);
        for (uint64_t i = 0; i < ByteCount(*previewWave); ++i)
        {
            const auto depth = pattern[i % pattern.size()];
            if (!depth) continue;
            const auto mask = BitStream::GetMask(0, depth - 1);
            bytes[i] = uint8_t((bytes[i] & ~mask) | (Random::GetByte() & mask));
        }
    }
    previewSound = new Sound(LoadSoundFromWave(*previewWave));
}

void SecFileUI::UpdateModelPreview()
{
    OwnedModel model(CloneModel(*secFile->loadedModel));
    if (shader)
        for (int i = 0; i < model->materialCount; ++i) model->materials[i].shader = *shader;
    if (secFile->bitDepth)
    {
        auto bytes = GetModelVertData(*model);
        const auto pattern = BitStream::GetByteLevelBitDepths(secFile->bitDepth, 4);
        for (std::size_t i = 0; i < bytes.size(); ++i)
        {
            const auto depth = pattern[i % pattern.size()];
            if (!depth) continue;
            const auto mask = BitStream::GetMask(0, depth - 1);
            bytes[i] = uint8_t((bytes[i] & ~mask) | (Random::GetByte() & mask));
        }
        // NaN/Inf cannot be displayed. Keep preview geometry finite at destructive depths.
        for (std::size_t offset = 0; offset < bytes.size(); offset += sizeof(float))
        {
            float value;
            std::memcpy(&value, bytes.data() + offset, sizeof(float));
            if (!std::isfinite(value))
            {
                value = 0;
                std::memcpy(bytes.data() + offset, &value, sizeof(float));
            }
        }
        SetModelVertData(*model, bytes);
    }
    previewModel = model.release();
}

void SecFileUI::DrawOpenButton()
{
    if (GuiButton({padding, screenSize.y - padding - 25, 100, 25}, "Open")) secFile->LoadContainer();
}

void SecFileUI::DrawSaveButton()
{
    if (GuiButton({padding, screenSize.y - padding - 25, 100, 25}, "Save")) secFile->SaveLoaded();
}

void SecFileUI::DrawContainer()
{
    const Rectangle panel{screenSize.x - padding - 300, padding, 300, screenSize.y - padding * 2};
    DrawRectangleRec(panel, DARKBROWN);
    DrawTextEx(*font, "Embedded files", {panel.x + padding, panel.y + padding}, 18, 1, WHITE);
    DrawTextEx(*font, "Drop files into this panel", {panel.x + padding, panel.y + 36}, 14, 1, LIGHTGRAY);
    const auto& files = secFile->container->files;
    const auto rows = std::size_t(std::max(1, int((panel.height - 65) / 32)));
    const auto lastStart = files.size() > rows ? files.size() - rows : 0;
    if (CheckCollisionPointRec(GetMousePosition(), panel))
    {
        const int wheel = int(GetMouseWheelMove());
        if (wheel < 0) firstVisibleFile = std::min(lastStart, firstVisibleFile + std::size_t(-wheel));
        else firstVisibleFile -= std::min(firstVisibleFile, std::size_t(wheel));
    }
    firstVisibleFile = std::min(firstVisibleFile, lastStart);
    for (std::size_t i = firstVisibleFile; i < files.size(); ++i)
    {
        const float y = panel.y + 65 + float(i - firstVisibleFile) * 32;
        if (y + 25 > panel.y + panel.height) break;
        const auto name = files[i].fileName.size() > 20 ? files[i].fileName.substr(0, 17) + "..." : files[i].fileName;
        DrawTextEx(*font, name.c_str(), {panel.x + padding, y + 5}, 14, 1, WHITE);
        if (GuiButton({panel.x + 200, y, 40, 25}, "Del"))
        {
            secFile->RemoveFileAt(i);
            break;
        }
        if (GuiButton({panel.x + 245, y, 40, 25}, "Exp")) secFile->ExportFileAt(i);
    }
}

void SecFileUI::DrawSize()
{
    const auto capacity = secFile->CapacityBytes();
    const auto used = secFile->container ? secFile->container->GetSize() : 0;
    const auto text = std::to_string(used) + " / " + std::to_string(capacity) + " bytes";
    DrawTextEx(*font, text.c_str(), {padding + 400, screenSize.y - padding - 20}, 14, 1, used > capacity ? RED : GREEN);
}

void SecFileUI::SetFont(Font* value) { font = value; }
void SecFileUI::SetShader(Shader* value) { shader = value; }

std::vector<float> SecFileUI::ExtractSizedSamples(const Wave* wave, int sampleCount)
{
    if (sampleCount <= 0) return {};
    std::vector<float> results(static_cast<std::size_t>(sampleCount), 0);
    if (!wave || !wave->data || !wave->frameCount || !wave->channels) return results;
    std::unique_ptr<float, decltype(&UnloadWaveSamples)> samples(LoadWaveSamples(*wave), UnloadWaveSamples);
    if (!samples) return results;
    float highest = 0;
    for (std::size_t i = 0; i < results.size(); ++i)
    {
        const auto start = uint64_t(i) * wave->frameCount / results.size();
        const auto end = std::min<uint64_t>(wave->frameCount,
            std::max(start + 1, uint64_t(i + 1) * wave->frameCount / results.size()));
        double sum = 0;
        for (auto frame = start; frame < end; ++frame)
            for (unsigned channel = 0; channel < wave->channels; ++channel)
                sum += samples.get()[frame * wave->channels + channel];
        results[i] = float(sum / double((end - start) * wave->channels));
        highest = std::max(highest, std::abs(results[i]));
    }
    if (highest > 0) for (auto& result : results) result /= highest;
    return results;
}

void SecFileUI::UnloadPreviews()
{
    if (previewTexture) { UnloadTexture(*previewTexture); delete previewTexture; previewTexture = nullptr; }
    if (previewSound) { StopSound(*previewSound); UnloadSound(*previewSound); delete previewSound; previewSound = nullptr; }
    if (previewWave) { UnloadWave(*previewWave); delete previewWave; previewWave = nullptr; }
    if (previewModel) { UnloadOwnedModel(*previewModel); delete previewModel; previewModel = nullptr; }
    if (meshTexture) { UnloadRenderTexture(*meshTexture); delete meshTexture; meshTexture = nullptr; }
    previewWaveData.clear();
    isAudioPlaying = false;
}
