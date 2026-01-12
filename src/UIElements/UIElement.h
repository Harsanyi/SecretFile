#pragma once
#include "raylib.h"

class UIElement
{
public:
    Rectangle rect;
    
    UIElement(){rect = Rectangle(0, 0, 100, 100);}
    
    virtual void Draw() = 0;
};
