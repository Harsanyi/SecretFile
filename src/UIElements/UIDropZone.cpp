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
    dropImage.transform.position = {150,100};
    dropImage.transform.size = {200,200};
    dropImage.color = BLACK;

    dropImage.SetParent(this);
}

void UIDropZone::Draw()
{
    DrawRectangle(transform.position.x, transform.position.y, transform.size.x, transform.size.y, WHITE);
    
    DrawTexturePro(dropTexture, Rectangle{0.,0.,(float)dropTexture.width, (float)dropTexture.height}, Rectangle{}, Vector2{0.5,0.5}, 0., BLACK);
    dropImage.Draw();
    
    if (IsFileDropped())
    {
        auto files = LoadDroppedFiles();
        if (CheckCollisionPointRec(GetMousePosition(), transform.GetRect()))
        {
            onFileDropped.Invoke(files.paths[0]);
        }

        UnloadDroppedFiles(files);
    }
}
