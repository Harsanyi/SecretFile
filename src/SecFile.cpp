#include "SecFile.h"

#include <ctime>
#include <format>
#include <iostream>
#include <string>

#include "Random.h"

const uint8_t SecFile::BIT_MASKS[]={
    0b0,
    0b1,
    0b11,
    0b111,
    0b1111,
    0b11111,
    0b111111,
    0b1111111,
    0b11111111
};

void SecFile::HideData(float* data)
{
    
}

void SecFile::Randomize(float& val)
{
    
}

void SecFile::Randomize(Image& orig, Image& newImg)
{
    newImg = ImageCopy(orig);
    Color* cols = (Color*)newImg.data;
    Randomize(cols, newImg.width*newImg.height);
}

void SecFile::Randomize(Color*& cols, int count)
{
    for (int c=0; c<count; c++)
    {
        //cols[c].r = Random::RandomizeByte(cols[c].r, 1);
        //cols[c].g = Random::GetChar();
        //cols[c].b = Random::GetChar();
        //cols[c].a = Random::GetChar();
    }
}

void SecFile::WriteData(uint8_t& target, uint8_t data, const int bits)
{
    data &= BIT_MASKS[bits];
    target &= ~BIT_MASKS[bits];
    target |= data;
}

void SecFile::WriteData(uint8_t*& target, const uint8_t* data, const int bits)
{
    
}

char SecFile::ReadData(uint8_t source, const int bits)
{
    return source & BIT_MASKS[bits];
}

void SecFile::CharReadWriteTest()
{
    int amount = 100;
    int depth = 4;
    uint8_t* container = Random::GetBytes(amount);
    uint8_t* values = Random::GetBytes(amount);

    int failed = 0;
    for (int i = 0; i < amount; i++)
    {
        uint8_t realData = values[i] & BIT_MASKS[depth];
        WriteData(container[i], values[i], depth);
        char readed = ReadData(container[i], depth);
        if (realData != readed)
        {
            failed++;
            std::cout << std::format("FAILED: write:{} read:{} \n", realData, readed);
            continue;
        }
    }

    std::cout << std::format("Test run for:{} failed:{}\n", amount, failed);
}
