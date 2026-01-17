#include "UIImageDropZone.h"

#include "Extensions.h"
#include "filesystem"
namespace fs = std::filesystem;

void UIImageDropZone::OnFileDropped(char* filePath)
{
    fs::path path(filePath);
    std::string ext = path.extension().string();
    if (!Contains(FORMATS, ext))return;
    
    texture = new Texture2D(LoadTexture(filePath));
    haveTexture = true;
    onImageDropped.Invoke(texture);
}

UIImageDropZone::UIImageDropZone()
{
    onFileDropped.Add([this](char* filePath)
    {
        OnFileDropped(filePath);
    });
}

void UIImageDropZone::Draw()
{
    if (haveTexture)
    {
        DrawTexturePro(
            *texture,
            Rectangle{0,0, (float)texture->width, (float)texture->height},
            transform.GetGlobalRect(),
            {0.5,0.5},
            0,
            WHITE
            );
        CheckFileDropped();
    }else
    {
        UIDropZone::Draw();
    }
}

void UIImageDropZone::SetTexture(Texture2D* texture)
{
    this->texture = texture;
}
