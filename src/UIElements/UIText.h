#pragma once
#include <string>

#include "UIElement.h"

class UIText : public UIElement
{
public:
    std::string text;
    
    void Draw() override;
};
