#pragma once
#include <vector>
#include "raylib.h"

class SecFile
{
private:
    static const uint8_t BIT_MASKS[];
    
public:
    static void HideData(float* data);

    static void Randomize(float& val);

    static void Randomize(Image& orig, Image& newImg);

    static void Randomize(Color*& cols, int count);
    
    static void WriteData(uint8_t& target, const uint8_t data, const int bits);

    static void WriteData(uint8_t*& target, const uint8_t* data, const int bits);

    static char ReadData(uint8_t source, const int bits);

    static void CharReadWriteTest();
};
