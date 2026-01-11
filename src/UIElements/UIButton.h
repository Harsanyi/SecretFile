#pragma once
#include <string>

#include "Event.h"
#include "UIElement.h"

class UIButton : public UIElement
{
public:
    std::string label;
    Event<> onClick;

    UIButton(){label = "Button";}
    
    void Draw() override;
};
