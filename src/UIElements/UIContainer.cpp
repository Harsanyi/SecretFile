#include "UIContainer.h"

UIContainer::UIContainer()
{
    layout.transform.SetParent(&this->transform);
}

void UIContainer::Draw()
{
    Rectangle rect = transform.GetGlobalRect();
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, WHITE);
    
    if (container != nullptr)
    {
        
    }
}
