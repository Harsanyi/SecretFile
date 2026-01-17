#include "UIImageDropZone.h"

#include "Extensions.h"
#include "filesystem"
namespace fs = std::filesystem;

void UIImageDropZone::OnFileDropped(char* filePath)
{
    fs::path path(filePath);
    std::string ext = path.extension().string();
    if (!Contains(FORMATS, ext))return;
    
    droppedTexture = LoadTexture(filePath);
    haveTexture = true;
    onImageDropped.Invoke(&droppedTexture);
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
            droppedTexture,
            Rectangle{0,0, (float)droppedTexture.width, (float)droppedTexture.height},
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
