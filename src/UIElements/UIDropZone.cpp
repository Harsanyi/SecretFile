#include "UIDropZone.h"

Texture2D UIDropZone::dropTexture;

UIDropZone::UIDropZone()
{
    if (!IsTextureValid(dropTexture))
    {
        dropTexture = LoadTexture("drop-icon.png");
    }

    dropImage = UIImage();
    dropImage.texture = dropTexture;
    dropImage.rect = rect;
    dropImage.color = BLACK;
}

void UIDropZone::Draw()
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
    
    DrawTexturePro(dropTexture, Rectangle{0.,0.,(float)dropTexture.width, (float)dropTexture.height}, Rectangle{}, Vector2{0.5,0.5}, 0., BLACK);
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
