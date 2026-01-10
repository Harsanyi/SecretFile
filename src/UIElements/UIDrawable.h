#pragma once
#include "UITransform.h"

struct UIDrawable : UITransform
{
protected:
    ~UIDrawable() = default;

public:
    virtual void Draw() = 0;
};
