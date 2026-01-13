#pragma once
#include <vector>

#include "raylib.h"
#include "UIRectTransform.h"

class UIElement
{
public:
    UIRectTransform transform = UIRectTransform();
    
    virtual void Draw() = 0;
};
