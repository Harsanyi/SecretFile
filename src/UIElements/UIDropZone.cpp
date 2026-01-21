#include "UIDropZone.h"
#include "filesystem"
namespace fs = std::filesystem;

Texture2D UIDropZone::dropTexture;

UIDropZone::UIDropZone()
{
    if (!IsTextureValid(dropTexture))
    {
        dropTexture = LoadTexture("drop-icon.png");
    }
    
    dropImage.texture = dropTexture;
    dropImage.transform.SetPivot({0.5,0.5});
    dropImage.transform.SetPosition({150,100});
    dropImage.transform.SetSize({100,100});
    dropImage.transform.SetParent(&this->transform);
    dropImage.color = BLACK;
}

void UIDropZone::Draw()
{
    Rectangle rect = transform.GetGlobalRect();
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
    dropImage.Draw();
    CheckFileDropped();
}

void UIDropZone::CheckFileDropped()
{
    if (IsFileDropped())
    {
        auto files = LoadDroppedFiles();
        if (CheckCollisionPointRec(GetMousePosition(), transform.GetGlobalRect()))
        {
            auto path = files.paths[0];
            std::cout << "File Dropped:"<<path<<std::endl;
            onFileDropped.Invoke(path);
            UnloadDroppedFiles(files);
        }
    }
}
