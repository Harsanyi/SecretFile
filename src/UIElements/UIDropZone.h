#pragma once
#include "UIDrawable.h"

struct UIDropZone : UIDrawable
{
    virtual ~UIDropZone() = default;
public:
    Color color;
    
    void Draw() override;
};
