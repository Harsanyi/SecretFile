#pragma once
#include <vector>
#include "raylib.h"

class SecFile
{
private:
    
public:
    static void WriteDataToImage(Image& img, const std::vector<float>& data, int bitDepth);
};
