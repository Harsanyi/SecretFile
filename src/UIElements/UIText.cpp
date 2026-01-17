#include "UIText.h"

void UIText::Draw()
{
    Rectangle rect = transform.GetGlobalRect();
    DrawText(text.c_str(), rect.x, rect.y, 14, WHITE);
}
