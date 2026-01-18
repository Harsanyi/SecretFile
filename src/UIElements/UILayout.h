#pragma once
#include "UIElement.h"

class UILayout : public UIElement
{
public:
    struct Padding
    {
        int left = 0;
        int right = 0;
        int top = 0;
        int bottom = 0;
    };
    enum Align{HORIZONTAL, VERTICAL};

    Align align = Align::HORIZONTAL;
    Padding padding = Padding();
    int space = 0;
    
    void Draw() override;
};
