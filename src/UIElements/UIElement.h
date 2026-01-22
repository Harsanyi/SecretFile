#pragma once
#include <vector>

#include "raylib.h"
#include "UIRectTransform.h"

class UIElement
{
private:
    static std::vector<UIElement*> instances;
    
public:
    UIElement();
    virtual ~UIElement();
    UIRectTransform transform;
    bool enabled = true;
    
    virtual void Draw() = 0;
    static void DrawAll();
};
