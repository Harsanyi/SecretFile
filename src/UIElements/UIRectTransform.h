#pragma once
#include "raylib.h"

class UIRectTransform
{
public:
    Vector2 position {0.,0.};
    Vector2 size {100.,100.};
    Vector2 pivot {0.,0.};

    Rectangle GetRect();
};
