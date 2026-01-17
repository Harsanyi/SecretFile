#pragma once
#include "Container.h"
#include "UIDropZone.h"
#include "UIElement.h"

class UIContainer : public UIDropZone
{
public:
    void Draw() override;

    Container* container;
};
