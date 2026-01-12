#pragma once
#include <vector>

#include "raylib.h"
#include "UIRectTransform.h"

class UIElement
{
private:
    std::vector<UIElement*> childs = std::vector<UIElement*>(0);
    UIElement* parent = nullptr;

    void RemoveChild(UIElement* child);
    void AddChild(UIElement* child);
    bool IsChild(UIElement* child);
    
public:
    UIRectTransform transform = UIRectTransform();
    
    virtual void Draw() = 0;

    void SetParent(UIElement* parent);
};
