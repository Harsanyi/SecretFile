#include "UILayout.h"

void UILayout::FitHorizontal()
{
    float width = padding.left + padding.right;
    if (align == UILayout::HORIZONTAL)
    {
        width += space * transform.ChildCount();
        for (int c=0; c<transform.ChildCount(); c++)
        {
            width += transform.GetChildAt(c)->GetSize().x;
        }
    }else
    {
        float widest = 0;
        for (int c=0; c<transform.ChildCount(); c++)
        {
            widest = std::max(widest, transform.GetChildAt(c)->GetSize().x);
        }
        width += widest;
    }

    transform.SetSize({width, transform.GetSize().y});
}

void UILayout::FitVertical()
{
    float height = padding.top + padding.bottom;
    if (align == UILayout::VERTICAL)
    {
        height += space * transform.ChildCount();
        for (int c=0; c<transform.ChildCount(); c++)
        {
            height += transform.GetChildAt(c)->GetSize().y;
        }
    }else
    {
        float highest = 0;
        for (int c=0; c<transform.ChildCount(); c++)
        {
            highest = std::max(highest, transform.GetChildAt(c)->GetSize().y);
        }
        height += highest;
    }

    transform.SetSize({transform.GetSize().x, height});
}

void UILayout::Draw()
{
    float x = padding.left;
    float y = padding.top;
    for (int c=0; c<transform.ChildCount(); c++)
    {
        UIRectTransform* child = transform.GetChildAt(c);
        child->SetPivot({0,0});
        child->SetPosition({x,y});

        Vector2 childSize = child->GetSize();
        if (align == Align::VERTICAL)
        {
            y+=childSize.y;
            y+=space;
        }else
        {
            x+=childSize.x;
            x+=space;
        }
    }

    //if (fitHorizontal)FitHorizontal();
    //if (fitVertical)FitVertical();
}
