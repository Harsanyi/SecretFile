#include "UIDropZone.h"

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
    
    if (IsFileDropped())
    {
        auto files = LoadDroppedFiles();
        if (CheckCollisionPointRec(GetMousePosition(), rect))
        {
            onFileDropped.Invoke(files.paths[0]);
        }

        UnloadDroppedFiles(files);
    }
}
