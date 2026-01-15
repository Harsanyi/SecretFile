#pragma once
#include <vector>

#include "raylib.h"
#include "UIRectTransform.h"

class UIElement
{
public:
    virtual ~UIElement() = default;
    UIRectTransform transform;
    
    virtual void Draw() = 0;
};
