#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "SecFile.h"
#include "SecFileUI.h"
#include "Extensions.h"
#include "Obj.h"
#include <cmath>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

namespace fs = std::filesystem;
static unsigned checks = 0;
static void Check(bool value, const char* message)
{
    ++checks;
    if (!value) throw std::runtime_error(message);
}
template<class Function> static void Reject(Function run)
{
    bool rejected = false;
    try { run(); } catch (const std::exception&) { rejected = true; }
    Check(rejected, "Expected operation to be rejected");
}
static Container Payload()
{
    Container container;
    container.files.emplace_back("notes.txt", std::vector<uint8_t>{'S', 'e', 'c', 'r', 'e', 't', 'F', 'i', 'l', 'e'});
    container.files.emplace_back("bytes.bin", std::vector<uint8_t>{0, 1, 127, 128, 254, 255});
    container.files.emplace_back("empty.txt", std::vector<uint8_t>{});
    return container;
}
static void Compare(const Container& expected, Container* decoded)
{
    std::unique_ptr<Container> actual(decoded);
    Check(actual && actual->files.size() == expected.files.size(), "Container file count differs");
    for (std::size_t i = 0; i < expected.files.size(); ++i)
    {
        Check(actual->files[i].fileName == expected.files[i].fileName, "Filename differs");
        Check(actual->files[i].data == expected.files[i].data, "Payload bytes differ");
    }
}

static void Images(const fs::path& output)
{
    const auto payload = Payload();
    for (const auto* extension : {".png", ".jpg", ".jpeg"})
    {
        Image source = GenImageGradientLinear(96, 64, 0, {34, 114, 172, 255}, {240, 178, 64, 255});
        const auto sourcePath = output / (std::string("image") + extension);
        Check(ExportImage(source, sourcePath.string().c_str()), "Image fixture export failed");
        UnloadImage(source);
        for (int depth = 1; depth <= 8; ++depth)
        {
            Image image = LoadImage(sourcePath.string().c_str());
            SecFile::WriteContainerToImage(payload, image, static_cast<uint8_t>(depth));
            const auto path = output / "image_mod.png";
            Check(ExportImage(image, path.string().c_str()), "PNG export failed");
            UnloadImage(image);
            image = LoadImage(path.string().c_str());
            Compare(payload, SecFile::ReadContainerFromImage(image, static_cast<uint8_t>(depth)));
            UnloadImage(image);
        }
    }
    Container exact;
    exact.files.emplace_back("a", std::vector<uint8_t>{1, 2}); // 36 bytes including marker and metadata.
    Image small = GenImageColor(3, 3, WHITE);
    Check(exact.GetSize() == 36, "Container overhead size differs");
    SecFile::WriteContainerToImage(exact, small, 8);
    Compare(exact, SecFile::ReadContainerFromImage(small, 8));
    exact.files[0].data.push_back(3);
    const auto original = SecFile::ImageToByteVector(small);
    Reject([&] { SecFile::WriteContainerToImage(exact, small, 8); });
    Check(original == SecFile::ImageToByteVector(small), "Rejected write modified the carrier");
    Reject([&] { SecFile::WriteContainerToImage(exact, small, 0); });
    Reject([&] { std::unique_ptr<Container> invalid(SecFile::ReadContainerFromImage(small, 0)); });
    UnloadImage(small);
}

static Wave MakeWave(unsigned bits, unsigned channels, unsigned frames = 4096)
{
    Wave wave{frames, 44100, bits, channels, nullptr};
    wave.data = AllocateBytes(ByteCount(wave));
    for (uint64_t i = 0; i < uint64_t(frames) * channels; ++i)
    {
        const float sample = std::sin(float(i) * 0.05f) * 0.5f;
        if (bits == 8) static_cast<uint8_t*>(wave.data)[i] = uint8_t(128 + sample * 100);
        else if (bits == 16) static_cast<int16_t*>(wave.data)[i] = int16_t(sample * 32767);
        else static_cast<float*>(wave.data)[i] = sample;
    }
    return wave;
}

static void Audio(const fs::path& output)
{
    const auto payload = Payload();
    for (unsigned bits : {8, 16, 32})
        for (unsigned channels : {1, 2})
        {
            Wave source = MakeWave(bits, channels);
            const auto input = output / ("audio_" + std::to_string(bits) + "_" + std::to_string(channels) + ".wav");
            Check(ExportWave(source, input.string().c_str()), "WAV fixture export failed");
            UnloadWave(source);
            for (int depth : {1, 2, 7, 8, 9, 12, 16})
            {
                Wave wave = LoadWave(input.string().c_str());
                SecFile::WriteContainerToWave(payload, wave, static_cast<uint8_t>(depth));
                Check(wave.sampleSize == 16 && wave.channels == channels, "PCM conversion changed channels");
                const auto path = output / "audio_mod.wav";
                Check(ExportWave(wave, path.string().c_str()), "WAV export failed");
                UnloadWave(wave);
                wave = LoadWave(path.string().c_str());
                Check(wave.sampleSize == 16 && wave.channels == channels, "WAV reload format differs");
                Compare(payload, SecFile::ReadContainerFromWave(wave, static_cast<uint8_t>(depth)));
                UnloadWave(wave);
            }
        }
    Wave shortSilent = MakeWave(16, 2, 1);
    std::memset(shortSilent.data, 0, static_cast<std::size_t>(ByteCount(shortSilent)));
    const auto silent = SecFileUI::ExtractSizedSamples(&shortSilent, 1024);
    Check(silent.size() == 1024, "Short preview length differs");
    for (auto value : silent) Check(value == 0 && std::isfinite(value), "Silent preview is not finite zero");
    static_cast<int16_t*>(shortSilent.data)[0] = 1;
    const auto shortPreview = SecFileUI::ExtractSizedSamples(&shortSilent, 1024);
    for (auto value : shortPreview) Check(std::isfinite(value), "Very short preview is not finite");
    Check(SecFileUI::ExtractSizedSamples(&shortSilent, 0).empty(), "Zero-width preview should be empty");
    UnloadWave(shortSilent);
}

static void CreateObj(const fs::path& path)
{
    std::ofstream file(path);
    file << std::setprecision(std::numeric_limits<float>::max_digits10);
    for (int triangle = 0; triangle < 300; ++triangle)
    {
        const float x = 1.25f + float(triangle % 20) * 0.1f;
        const float y = 1.5f + float(triangle / 20) * 0.1f;
        file << "v " << x << ' ' << y << " 1.125\n";
        file << "v " << x + 0.08f << ' ' << y << " 1.125\n";
        file << "v " << x << ' ' << y + 0.08f << " 1.125\n";
    }
    for (int i = 0; i < 300; ++i) file << "f " << i * 3 + 1 << ' ' << i * 3 + 2 << ' ' << i * 3 + 3 << '\n';
}

static void Models(const fs::path& output)
{
    const auto input = output / "triangles.obj";
    CreateObj(input);
    const auto payload = Payload();
    for (int depth : {1, 2, 4, 8, 12, 16})
    {
        OwnedModel model(new Model(LoadModel(input.string().c_str())));
        Check(model->meshCount == 1, "OBJ fixture should have one mesh");
        const auto before = GetModelVertData(*model);
        OwnedModel clone(CloneModel(*model));
        SecFile::WriteContainerToModel(payload, *clone, static_cast<uint8_t>(depth));
        Check(before == GetModelVertData(*model), "Clone write modified source model");
        const auto expectedVertices = GetModelVertData(*clone);
        const auto path = output / "triangles_mod.obj";
        Obj::SaveToFile(*clone, path.string());
        OwnedModel restored(new Model(LoadModel(path.string().c_str())));
        Check(GetModelVertData(*restored) == expectedVertices, "OBJ float bytes changed during text save/reload");
        Compare(payload, SecFile::ReadContainerFromModel(*restored, static_cast<uint8_t>(depth)));
    }
    Mesh mesh{};
    mesh.vertexCount = 4;
    mesh.triangleCount = 2;
    mesh.vertices = static_cast<float*>(AllocateBytes(12 * sizeof(float)));
    const float positions[]{1, 1, 1, 2, 1, 1, 2, 2, 1, 1, 2, 1};
    std::memcpy(mesh.vertices, positions, sizeof(positions));
    mesh.indices = static_cast<unsigned short*>(AllocateBytes(6 * sizeof(unsigned short)));
    const unsigned short indices[]{0, 1, 2, 0, 2, 3};
    std::memcpy(mesh.indices, indices, sizeof(indices));
    UploadMesh(&mesh, true);
    OwnedModel indexed(new Model(LoadModelFromMesh(mesh)));
    {
        OwnedModel clone(CloneModel(*indexed));
        Check(clone->meshes[0].indices != indexed->meshes[0].indices, "Indices are shared");
        Check(std::memcmp(clone->meshes[0].indices, indices, sizeof(indices)) == 0, "Index clone differs");
        Check(clone->materials[0].maps != indexed->materials[0].maps, "Material maps are shared");
        Obj::SaveToFile(*clone, (output / "indexed.obj").string());
    }
    Check(std::memcmp(indexed->meshes[0].indices, indices, sizeof(indices)) == 0, "Destroying clone damaged original");
    std::ifstream faces(output / "indexed.obj");
    const std::string text((std::istreambuf_iterator<char>(faces)), {});
    Check(text.find("f 1 2 3\nf 1 3 4\n") != std::string::npos, "OBJ face indices are not one-based");
}

static void FileSafetyAndUi(const fs::path& root, const fs::path& output)
{
    Check(SecFile::SafeExportName("../../safe.txt") == "safe.txt", "Unix traversal was not stripped");
    Check(SecFile::SafeExportName("C:\\private\\safe.txt") == "safe.txt", "Windows traversal was not stripped");
    for (const auto* name : {"..", "", "NUL.txt", "a:stream", "trailing.", "bad?name"})
    {
        Reject([&] { SecFile::SafeExportName(name); });
    }
    const auto payload = output / "payload.txt";
    { std::ofstream file(payload); file << "SecretFile round-trip example\n"; }
    SecFile file;
    SecFileUI ui(&file, {1280, 720});
    Font font = GetFontDefault();
    GuiLoadStyle((root / "resources/styles/jungle/style_jungle.rgs").string().c_str());
    ui.SetFont(&font);
    file.LoadModelFile((output / "triangles.obj").string());
    const auto vertices = GetModelVertData(*file.loadedModel);
    for (uint8_t depth : {0, 1, 2, 8, 0})
    {
        file.bitDepth = depth;
        ui.UpdatePreview();
        ui.UpdateModelTexture();
        Check(vertices == GetModelVertData(*file.loadedModel), "Preview modified original geometry");
    }
    ui.UnloadPreviews();
    file.LoadAudioFile((output / "audio_16_2.wav").string());
    Check(!file.loadedModel && !file.loadedTexture && file.loadedWave, "Audio switch kept another carrier");
    for (uint8_t depth : {0, 1, 9, 16, 0}) { file.bitDepth = depth; ui.UpdatePreview(); }
    ui.UnloadPreviews();
    Image demo = GenImageGradientLinear(960, 540, 35, {22, 66, 104, 255}, {70, 174, 180, 255});
    ImageDrawCircle(&demo, 700, 250, 150, {242, 183, 80, 255});
    ImageDrawCircle(&demo, 700, 250, 95, {30, 82, 113, 255});
    ImageDrawRectangle(&demo, 70, 100, 390, 320, {25, 52, 77, 255});
    ImageDrawText(&demo, "SecretFile", 100, 145, 44, RAYWHITE);
    ImageDrawText(&demo, "IMAGE CARRIER", 100, 220, 28, {108, 213, 213, 255});
    ImageDrawText(&demo, "Low-order bit embedding", 100, 290, 20, RAYWHITE);
    Check(ExportImage(demo, (output / "image.png").string().c_str()), "Demo carrier export failed");
    UnloadImage(demo);
    file.LoadImageFile((output / "image.png").string());
    Check(!file.loadedModel && !file.loadedWave && file.loadedTexture, "Image switch kept another carrier");
    file.bitDepth = 2;
    file.LoadContainer();
    file.AddFileFromPath(payload.string());
    file.AddFileFromPath((output / "indexed.obj").string());
    ui.UpdatePreview();
    const auto currentDirectory = fs::current_path();
    fs::current_path(root / "resources");
    file.SaveLoaded();
    Check(fs::exists(output / "image_mod.png"), "Save did not use carrier directory");
    file.ExportFileAt(0);
    Check(fs::exists(output / "image_extracted/payload.txt"), "Extraction did not use carrier directory");
    Reject([&] { file.ExportFileAt(0); });
    file.container->files[0].fileName = "../../escape.txt";
    file.ExportFileAt(0);
    Check(fs::exists(output / "image_extracted/escape.txt") && !fs::exists(output / "escape.txt"), "Extraction escaped its directory");
    file.container->files[0].fileName = "payload.txt";
    file.bitDepth = 0;
    Reject([&] { file.SaveLoaded(); });
    Reject([&] { file.LoadContainer(); });
    file.bitDepth = 2;
    file.status = "Image carrier ready. Bit depth 2; two arbitrary files in the container.";
    for (int i = 0; i < 4; ++i)
    {
        BeginDrawing();
        ClearBackground({44, 44, 44, 255});
        ui.Draw();
        EndDrawing();
    }
    fs::current_path(output);
    Image screenshot{rlReadScreenPixels(1280, 720), 1280, 720, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
    const bool screenshotSaved = ExportImage(screenshot, (output / "screenshot.png").string().c_str());
    UnloadImage(screenshot);
    Check(screenshotSaved, "Application screenshot was not saved");
    fs::current_path(currentDirectory);
}

int main(int argc, char** argv)
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    const auto root = fs::absolute(argc > 1 ? fs::path(argv[1]) : fs::current_path());
    const auto run = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    const auto output = root / "tests/output" / run;
    fs::create_directories(output);
    std::cout << "Test output: " << output.string() << '\n';
    { std::ofstream latest(root / "tests/output/latest-run.txt"); latest << output.string(); }
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1280, 720, "SecretFile carrier checks");
    InitAudioDevice();
    int result = 0;
    try
    {
        std::cout << "Images\n"; Images(output);
        std::cout << "Audio\n"; Audio(output);
        std::cout << "Models\n"; Models(output);
        std::cout << "File safety and UI\n"; FileSafetyAndUi(root, output);
        std::cout << "Carrier tests passed: " << checks << " assertions; PNG/WAV/OBJ disk round-trips and UI lifecycle.\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "Carrier check failed: " << error.what() << '\n';
        result = 1;
    }
    CloseAudioDevice();
    CloseWindow();
    return result;
}
