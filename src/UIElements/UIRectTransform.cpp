#include "UIRectTransform.h"

#include <iostream>
#include <__msvc_ostream.hpp>

#include "Extensions.h"
#include "UIImage.h"

Rectangle UIRectTransform::GetLocalRect() const
{
    return rect;
}

Rectangle UIRectTransform::GetGlobalRect() const
{
    if (parent == nullptr)
    {
        return rect;
    }

    Rectangle parentRect = parent->GetGlobalRect();
    return {parentRect.x + rect.x, parentRect.y + rect.y, rect.width, rect.height};
}

void UIRectTransform::SetSize(const Vector2& locSize)
{
    Vector2 pos = GetPosition();
    rect.width = locSize.x;
    rect.height = locSize.y;
    SetPosition(pos);
}

Vector2 UIRectTransform::GetSize() const
{
    return {rect.width, rect.height};
}

int UIRectTransform::ChildCount() const
{
    return childs.size();
}

UIRectTransform* UIRectTransform::GetChildAt(int i) const
{
    return childs[i];
}

void UIRectTransform::SetPosition(const Vector2& locPosition)
{
    rect.x = locPosition.x - rect.width * pivot.x;
    rect.y = locPosition.y - rect.height * pivot.y;
}

Vector2 UIRectTransform::GetPosition() const
{
    return {rect.x + rect.width * pivot.x,rect.y + rect.height * pivot.y};
}

void UIRectTransform::SetPivot(const Vector2& locPivot)
{
    Vector2 pos = GetPosition();
    pivot = locPivot;
    SetPosition(pos);
}

void UIRectTransform::RemoveChild(UIRectTransform* child)
{
    childs.erase(std::remove(childs.begin(), childs.end(), child), childs.end());
}

void UIRectTransform::AddChild(UIRectTransform* child)
{
    childs.insert(std::remove(childs.begin(), childs.end(), child), child);
}

bool UIRectTransform::IsChild(UIRectTransform* child)
{
    for (int c=0; c<childs.size(); c++)
    {
        if (childs.at(c) == child)return true;
        if (childs.at(c)->IsChild(child)) return true;
    }

    return false;
}

void UIRectTransform::SetParent(UIRectTransform* newParent)
{
    if (newParent == this)
    {
        std::cout << "Error : You cannot set parent for itself!";
        return;
    }

    if (this->IsChild(newParent))
    {
        std::cout << "Error : Circular child chain!";
        return;
    }
    
    if (parent != nullptr)
    {
        parent->RemoveChild(parent);
    }

    parent = newParent;
    newParent->AddChild(this);
}
