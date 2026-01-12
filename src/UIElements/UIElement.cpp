#include "UIElement.h"

#include <iostream>

void UIElement::RemoveChild(UIElement* child)
{
    childs.erase(std::remove(childs.begin(), childs.end(), child), childs.end());
}

void UIElement::AddChild(UIElement* child)
{
    childs.insert(std::remove(childs.begin(), childs.end(), child), child);
}

bool UIElement::IsChild(UIElement* child)
{
    for (int c=0; c<childs.size(); c++)
    {
        if (childs.at(c) == child)return true;
        if (childs.at(c)->IsChild(child)) return true;
    }

    return false;
}

void UIElement::SetParent(UIElement* newParent)
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
