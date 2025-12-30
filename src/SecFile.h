#pragma once
#include <vector>
#include "raylib.h"

class SecFile
{
public:
    static void HideData(float* data);

    static void Randomize(float& val);

    static void Randomize(Image& orig, Image& newImg);

    static void Randomize(Color*& cols, int count);
};
