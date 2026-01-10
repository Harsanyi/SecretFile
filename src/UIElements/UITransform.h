#pragma once
#include <vector>

#include "raylib.h"

struct UITransform
{
protected:
    ~UITransform() = default;

public:
    Rectangle rect;
};
