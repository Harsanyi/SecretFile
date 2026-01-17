#pragma once
#include <string>

#include "Event.h"
#include "UIElement.h"

class UISlider : public UIElement
{
public:
    float min=0;
    float max=1;
    float value=0.5;
    bool intValues = false;
    std::string label="Slider";

    Event<float> onValueChanged;
    
    void Draw() override;
};
