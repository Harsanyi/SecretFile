#pragma once
#include "UIElement.h"

class UIImage : public UIElement
{
public:
    Texture2D texture;
    Color color = WHITE;
    
    void Draw() override;
};
