#include "SecFile.h"
#include "Capacity.h"
#include "Extensions.h"
#include "Obj.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <stdexcept>

namespace fs = std::filesystem;

static void CheckLittleEndian()
{
    const uint16_t one = 1;
    if (*reinterpret_cast<const uint8_t*>(&one) != 1)
        throw std::runtime_error("Audio/model carriers currently require a little-endian host");
}

SecFile::~SecFile() { UnloadFiles(); ResetContainer(); }

std::vector<uint8_t> SecFile::ImageToByteVector(Image& image)
{
    if (!image.data || image.width <= 0 || image.height <= 0) throw std::invalid_argument("Invalid image");
    if (image.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
        ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    if (!image.data) throw std::runtime_error("RGBA image conversion failed");
    const auto* bytes = static_cast<const uint8_t*>(image.data);
    return {bytes, bytes + static_cast<std::size_t>(ByteCount(image))};
}

void SecFile::NormalizeWave(Wave& wave)
{
    CheckLittleEndian();
    if (!wave.data || !wave.frameCount || !wave.sampleRate || !wave.channels)
        throw std::invalid_argument("Invalid audio carrier");
    if (wave.sampleSize != 16) WaveFormat(&wave, wave.sampleRate, 16, wave.channels);
    if (!wave.data || wave.sampleSize != 16) throw std::runtime_error("16-bit PCM conversion failed");
}

Container* SecFile::ReadContainerFromTexture(const Texture2D& texture, uint8_t depth)
{
    Image image = LoadImageFromTexture(texture);
    try
    {
        auto* result = ReadContainerFromImage(image, depth);
        UnloadImage(image);
        return result;
    }
    catch (...) { UnloadImage(image); throw; }
}

Container* SecFile::ReadContainerFromImage(Image& image, uint8_t depth)
{
    auto bytes = ImageToByteVector(image);
    BitStream stream(bytes, BitStream::GetByteLevelBitDepths(depth, 1));
    return stream.ReadContainer();
}

Container* SecFile::ReadContainerFromWave(Wave& wave, uint8_t depth)
{
    NormalizeWave(wave);
    const auto* begin = static_cast<const uint8_t*>(wave.data);
    std::vector<uint8_t> bytes(begin, begin + static_cast<std::size_t>(ByteCount(wave)));
    BitStream stream(bytes, BitStream::GetByteLevelBitDepths(depth, 2));
    return stream.ReadContainer();
}

Container* SecFile::ReadContainerFromModel(const Model& model, uint8_t depth)
{
    CheckLittleEndian();
    auto bytes = GetModelVertData(model);
    BitStream stream(bytes, BitStream::GetByteLevelBitDepths(depth, 4));
    return stream.ReadContainer();
}

void SecFile::WriteContainerToImage(const Container& container, Image& image, uint8_t depth)
{
    auto bytes = ImageToByteVector(image);
    BitStream stream(bytes, BitStream::GetByteLevelBitDepths(depth, 1));
    stream.WriteContainer(container);
    std::memcpy(image.data, bytes.data(), bytes.size());
}

void SecFile::WriteContainerToWave(const Container& container, Wave& wave, uint8_t depth)
{
    NormalizeWave(wave);
    const auto* begin = static_cast<const uint8_t*>(wave.data);
    std::vector<uint8_t> bytes(begin, begin + static_cast<std::size_t>(ByteCount(wave)));
    BitStream stream(bytes, BitStream::GetByteLevelBitDepths(depth, 2));
    stream.WriteContainer(container);
    std::memcpy(wave.data, bytes.data(), bytes.size());
}

void SecFile::WriteContainerToModel(const Container& container, Model& model, uint8_t depth)
{
    CheckLittleEndian();
    if (model.meshCount > 0 && model.meshes[0].indices)
        throw std::invalid_argument("Embed in triangle-expanded OBJ geometry loaded by raylib");
    auto bytes = GetModelVertData(model);
    BitStream stream(bytes, BitStream::GetByteLevelBitDepths(depth, 4));
    stream.WriteContainer(container);
    for (std::size_t offset = 0; offset < bytes.size(); offset += sizeof(float))
    {
        float value;
        std::memcpy(&value, bytes.data() + offset, sizeof(float));
        if (!std::isfinite(value)) throw std::runtime_error("Bit depth produced non-finite vertex coordinates; use a lower depth");
    }
    SetModelVertData(model, bytes);
}

uint64_t SecFile::CapacityBytes() const
{
    if (loadedTexture) return CarrierCapacityBytes(ByteCount(*loadedTexture), 1, bitDepth);
    if (loadedWave) return CarrierCapacityBytes(CheckedMultiply(loadedWave->frameCount, loadedWave->channels), 2, bitDepth);
    if (loadedModel) return CarrierCapacityBytes(ByteCount(*loadedModel) / sizeof(float), 4, bitDepth);
    return 0;
}

void SecFile::LoadContainer()
{
    if (!HaveLoaded() || bitDepth == 0) throw std::runtime_error("Choose a nonzero bit depth before opening a container");
    std::unique_ptr<Container> result;
    if (loadedTexture) result.reset(ReadContainerFromTexture(*loadedTexture, bitDepth));
    else if (loadedWave) result.reset(ReadContainerFromWave(*loadedWave, bitDepth));
    else result.reset(ReadContainerFromModel(*loadedModel, bitDepth));
    const bool found = bool(result);
    if (!result) result = std::make_unique<Container>();
    ResetContainer();
    container = result.release();
    status = found ? "Embedded container opened." : "No container found; new empty container created.";
}

void SecFile::AddFileFromPath(const std::string& path)
{
    if (!container) throw std::runtime_error("Open a container first");
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) throw std::runtime_error("Cannot open payload file");
    const auto end = input.tellg();
    if (end < 0 || static_cast<uint64_t>(end) > std::vector<uint8_t>().max_size())
        throw std::length_error("Invalid payload file length");
    const auto name = fs::path(path).filename().string();
    const auto serializedSize = CheckedAdd(16, CheckedAdd(name.size(), static_cast<uint64_t>(end)));
    if (CheckedAdd(container->GetSize(), serializedSize) > CapacityBytes())
        throw std::length_error("Payload plus container metadata exceeds carrier capacity");
    std::vector<uint8_t> bytes(static_cast<std::size_t>(end));
    input.seekg(0);
    if (!bytes.empty() && !input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
        throw std::runtime_error("Cannot read payload file");
    container->files.emplace_back(name, std::move(bytes));
    status = "Added: " + name;
}

void SecFile::SaveLoaded()
{
    if (!HaveLoaded() || !container || bitDepth == 0) throw std::runtime_error("Choose a carrier, container and nonzero bit depth before saving");
    if (container->GetSize() > CapacityBytes()) throw std::length_error("Container plus metadata exceeds carrier capacity");
    const auto extension = loadedTexture ? ".png" : loadedWave ? ".wav" : ".obj";
    const auto output = loadedFilePath.parent_path() / (loadedFilePath.stem().string() + "_mod" + extension);
    if (loadedTexture)
    {
        Image image = LoadImageFromTexture(*loadedTexture);
        try
        {
            WriteContainerToImage(*container, image, bitDepth);
            if (!ExportImage(image, output.string().c_str())) throw std::runtime_error("PNG export failed");
        }
        catch (...) { UnloadImage(image); throw; }
        UnloadImage(image);
    }
    else if (loadedWave)
    {
        Wave wave = WaveCopy(*loadedWave);
        try
        {
            WriteContainerToWave(*container, wave, bitDepth);
            if (!ExportWave(wave, output.string().c_str())) throw std::runtime_error("WAV export failed");
        }
        catch (...) { UnloadWave(wave); throw; }
        UnloadWave(wave);
    }
    else
    {
        OwnedModel model(CloneModel(*loadedModel));
        WriteContainerToModel(*container, *model, bitDepth);
        Obj::SaveToFile(*model, output.string());
    }
    status = "Saved: " + output.string();
}

void SecFile::RemoveFileAt(std::size_t index)
{
    if (!container || index >= container->files.size()) throw std::out_of_range("File index out of range");
    container->files.erase(container->files.begin() + index);
}

fs::path SecFile::SafeExportName(const std::string& name)
{
    // Treat both separators as path separators, independent of the host OS.
    std::string normalized = name;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    const auto colon = normalized.find(':');
    if (colon != std::string::npos && (colon != 1 || normalized.size() < 3 || normalized[2] != '/'))
        throw std::runtime_error("Invalid drive-relative or stream filename");
    const auto filename = fs::path(normalized).filename();
    const auto text = filename.string();
    if (text.empty() || text == "." || text == ".." || text.back() == '.' || text.back() == ' ' ||
        text.find_first_of("<>:\"/\\|?*") != std::string::npos ||
        std::any_of(text.begin(), text.end(), [](unsigned char c) { return c < 32; }))
        throw std::runtime_error("Invalid embedded filename");
    std::string stem = filename.stem().string();
    std::transform(stem.begin(), stem.end(), stem.begin(), [](unsigned char c) { return char(std::toupper(c)); });
    if (stem == "CON" || stem == "PRN" || stem == "AUX" || stem == "NUL" ||
        (stem.size() == 4 && (stem.substr(0, 3) == "COM" || stem.substr(0, 3) == "LPT") && stem[3] >= '0' && stem[3] <= '9'))
        throw std::runtime_error("Reserved embedded filename");
    return filename;
}

void SecFile::ExportFileAt(std::size_t index)
{
    if (!container || index >= container->files.size()) throw std::out_of_range("File index out of range");
    const auto& file = container->files[index];
    const auto directory = loadedFilePath.parent_path() / (loadedFilePath.stem().string() + "_extracted");
    fs::create_directories(directory);
    const auto output = directory / SafeExportName(file.fileName);
    // Refuse links/overwrites, including dangling symlinks supplied beside the carrier.
    if (fs::is_symlink(fs::symlink_status(directory)) || fs::symlink_status(output).type() != fs::file_type::not_found)
        throw std::runtime_error("Extraction target already exists or is a link");
    std::ofstream stream(output, std::ios::binary);
    if (!stream || (file.data.size() && !stream.write(reinterpret_cast<const char*>(file.data.data()), static_cast<std::streamsize>(file.data.size()))))
        throw std::runtime_error("Cannot export embedded file");
    status = "Extracted: " + output.string();
}

void SecFile::UnloadFiles()
{
    if (loadedTexture) { UnloadTexture(*loadedTexture); delete loadedTexture; loadedTexture = nullptr; }
    if (loadedWave) { UnloadWave(*loadedWave); delete loadedWave; loadedWave = nullptr; }
    if (loadedModel) { UnloadOwnedModel(*loadedModel); delete loadedModel; loadedModel = nullptr; }
    loadedFilePath.clear();
}

void SecFile::LoadImageFile(const std::string& path)
{
    const auto fullPath = fs::absolute(path);
    Texture2D texture = LoadTexture(fullPath.string().c_str());
    if (!texture.id) throw std::runtime_error("Cannot load image carrier");
    UnloadFiles(); ResetContainer();
    loadedTexture = new Texture2D(texture);
    loadedFilePath = fullPath;
    bitDepth = std::min<uint8_t>(bitDepth, 8);
    status = "Image loaded.";
}

void SecFile::LoadAudioFile(const std::string& path)
{
    const auto fullPath = fs::absolute(path);
    Wave wave = LoadWave(fullPath.string().c_str());
    try { NormalizeWave(wave); } catch (...) { if (wave.data) UnloadWave(wave); throw; }
    UnloadFiles(); ResetContainer();
    loadedWave = new Wave(wave);
    loadedFilePath = fullPath;
    bitDepth = std::min<uint8_t>(bitDepth, 16);
    status = "Audio loaded as 16-bit PCM; channels preserved.";
}

void SecFile::LoadModelFile(const std::string& path)
{
    const auto fullPath = fs::absolute(path);
    OwnedModel model(new Model(LoadModel(fullPath.string().c_str())));
    ByteCount(*model);
    UnloadFiles(); ResetContainer();
    loadedModel = model.release();
    loadedFilePath = fullPath;
    status = "OBJ loaded; first mesh is the carrier.";
}

void SecFile::ResetContainer() { delete container; container = nullptr; }
bool SecFile::HaveLoaded() const { return loadedTexture || loadedWave || loadedModel; }
