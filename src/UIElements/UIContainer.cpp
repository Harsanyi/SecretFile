#include "UIContainer.h"

#include <fstream>

#include "filesystem"
#include "UIText.h"

namespace fs = std::filesystem;

void UIContainer::OnFileDropped(char* path)
{
    fs::path file(path);
    std::string name = file.stem().string();

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

    if (container == nullptr)
    {
        std::cerr << "Container is null" << std::endl;
        return;
    }
    container->files.emplace_back(fileData);
}

UIContainer::UIContainer()
{
    container = new Container();
    layout.transform.SetParent(&this->transform);
    onFileDropped.Add([this](char* filePath)
    {
        OnFileDropped(filePath);
    });
    dropImage.enabled = false;
}

void UIContainer::Draw()
{
    Rectangle rect = transform.GetGlobalRect();
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
    
    if (container != nullptr)
    {
        const auto& files = container->files;
        std::cout << "Found files:" << files.size() << std::endl;
        for (int f=0; f<files.size(); f++)
        {
            const FileData& file = files[f];

            //draw file
            UILayout* row = new UILayout();
            row->transform.SetParent(&this->transform);
            row->align = UILayout::HORIZONTAL;
            
            UIText* label = new UIText();
            label->transform.SetParent(&row->transform);
            label->text = file.fileName;

            std::cout << label->text << std::endl;
        }
    }

    CheckFileDropped();
}
