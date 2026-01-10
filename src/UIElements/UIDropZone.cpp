#include "raygui.h"
#include "UIDropZone.h"

void UIDropZone::Draw()
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
    if (IsFileDropped() && CheckCollisionPointRec(GetMousePosition(), rect))
    {
        
    }
}
