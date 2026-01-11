#include "UIDropZone.h"

void UIDropZone::Draw()
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
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
