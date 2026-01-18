#pragma once
#include "Container.h"
#include "UIDropZone.h"
#include "UIElement.h"
#include "UILayout.h"

class UIContainer : public UIDropZone
{
    UILayout layout;
    
public:
    UIContainer();
    
    void Draw() override;

    Container* container;
};
