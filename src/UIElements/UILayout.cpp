#include "UILayout.h"

void UILayout::Draw()
{
    float x = padding.left;
    float y = padding.top;
    for (int c=0; c<transform.ChildCount(); c++)
    {
        UIRectTransform* child = transform.GetChildAt(c);
        child->SetPivot({0,0});
        child->SetPosition({x,y});
        
        if (align == Align::VERTICAL)
        {
            y+=child->GetSize().y;
            y+=space;
        }else
        {
            x+=child->GetSize().x;
            x+=space;
        }
    }
}
