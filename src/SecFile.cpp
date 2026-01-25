#include "SecFile.h"

#include <fstream>
#include <iostream>
#include "filesystem"
#include "BitStream.h"
#include "Random.h"

namespace fs = std::filesystem;


void SecFile::WriteDataToImage(Image& img, const std::vector<uint8_t>& data, uint8_t bitDepth)
{
    auto colors = ImageToByteVector(img);
    BitStream bs(colors, bitDepth);
    uint64_t size = data.size();
    bs.WriteUInt64(size);
    bs.WriteBytes(data);
    std::memcpy(img.data, bs.data.data(), bs.data.size());
}

std::vector<uint8_t> SecFile::ReadDataFromImage(Image& img, uint8_t bitDepth)
{
    auto colors = ImageToByteVector(img);
    BitStream bs(colors, bitDepth);
    uint64_t size = bs.ReadUInt64();
    return  bs.ReadBytes(size);
}

std::vector<uint8_t> SecFile::ImageToByteVector(Image& img)
{
    if (img.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
    {
        ImageFormat(&img,PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }
    int byteCount = img.width * img.height * 4;
    uint8_t* colors = static_cast<uint8_t*>(img.data);
    return  std::vector<uint8_t>(colors, colors + byteCount);
}

long SecFile::GetSizeInBytes(int width, int height, uint8_t bitDepth)
{
    return width * height * 4 * bitDepth / 8;
}

void SecFile::WriteDataToTexture(Texture2D& texture, const std::vector<uint8_t>& data, uint8_t bitDepth)
{
    Image img = LoadImageFromTexture(texture);
    WriteDataToImage(img, data, bitDepth);
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
}

Container* SecFile::ReadContainerFromTexture(Texture2D& texture, uint8_t bitDepth)
{
    Image img = LoadImageFromTexture(texture);
    Container* cont = ReadContainerFromImage(img, bitDepth);
    UnloadImage(img);
    return cont;
}

Container* SecFile::ReadContainerFromImage(Image& img, uint8_t bitDepth)
{
    std::vector<uint8_t> imgData = ImageToByteVector(img);
    BitStream bs(imgData,bitDepth);
    return bs.ReadContainer();
}

void SecFile::WriteContainerToImage(Container& container, Image& img, uint8_t bitDepth)
{
    long imgSize = GetSizeInBytes(img.width, img.height, bitDepth);
    long contSize = container.GetSize();
    if (contSize >= imgSize)
    {
        std::cerr << "Container size:" << contSize << " is too big for img capacity: " << imgSize << std::endl; 
        return;
    }
    
    std::vector<uint8_t> imgData = ImageToByteVector(img);
    BitStream bs(imgData,bitDepth);
    bs.WriteContainer(container);
    std::memcpy(img.data, bs.data.data(), imgData.size());
}

void SecFile::LoadContainer()
{
    if (loadedTexture == nullptr)return;
    if (container != nullptr)
    {
        delete container;
    }
    container = ReadContainerFromTexture(*loadedTexture, bitDepth);
    if (container == nullptr)
    {
        container = new Container();
    }
}

void SecFile::AddFileFromPath(std::string path)
{
    std::cout << "Add file from path: " << path << std::endl;
    
    fs::path file(path);
    std::string name = file.filename().string();

    std::ifstream input(file, std::ios::binary);
    if (!input.is_open())
    {
        std::cerr << "Failed to open file " << path << std::endl;
        return;
    }

    input.seekg(0, std::ios::end);
    std::streamsize size = input.tellg();
    input.seekg(0, std::ios::beg);

    std::vector<uint8_t> bytes(size);
    if (!input.read(reinterpret_cast<char*>(bytes.data()), size))
    {
        std::cerr << "Failed to read file " << path << std::endl;
        return;
    }

    input.close();

    FileData fileData = FileData();
    fileData.fileName = name;
    fileData.data = bytes;

    container->files.push_back(fileData);
    std::cout << "File added:"<< fileData.fileName << std::endl;
    std::cout << "Files:"<< container->files.size() << std::endl;
}

void SecFile::SaveImage()
{
    std::cout << "Save image" << std::endl;
    if (loadedTexture == nullptr)return;

    fs::path path(loadedTextureName);
    std::stringstream ss;
    ss << path.stem().string() << "_mod.png";
    
    Image img = LoadImageFromTexture(*loadedTexture);
    WriteContainerToImage(*container, img, bitDepth);
    ExportImage(img, ss.str().c_str());
    UnloadImage(img);
}

void SecFile::RemoveFileAt(const int index)
{
    container->files.erase(container->files.begin() + index);
}

void SecFile::ExportFileAt(const int index)
{
    if (index >= container->files.size())
    {
        std::cerr << "File index out of range" << std::endl;
        return;
    }

    FileData file = container->files[index];
    SaveFileData(file.fileName.c_str(), file.data.data(), file.data.size());
    std::cout << "File saved:"<< file.fileName << std::endl;
}
