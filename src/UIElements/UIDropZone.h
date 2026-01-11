#pragma once
#include <string>

#include "Event.h"
#include "UIElement.h"

class UIDropZone : public UIElement
{
public:
    Event<std::string> onFileDropped;
    
    void Draw() override;
};
