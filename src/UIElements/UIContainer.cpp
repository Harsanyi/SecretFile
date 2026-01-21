#include "UIContainer.h"
#include "filesystem"
#include "UIText.h"

namespace fs = std::filesystem;

void UIContainer::OnFileDropped(char* path)
{
    
}

UIContainer::UIContainer()
{
    layout.transform.SetParent(&this->transform);
    onFileDropped.Add([this](char* filePath)
    {
        OnFileDropped(filePath);
    });
}

void UIContainer::Draw()
{
    Rectangle rect = transform.GetGlobalRect();
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
    
    if (container != nullptr)
    {
        const auto& files = container->files;
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
        }
    }

    CheckFileDropped();
}
