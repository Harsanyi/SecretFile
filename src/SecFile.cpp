#include "SecFile.h"

#include <ctime>

#include "Random.h"

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
    //newImg.data = cols;
}

void SecFile::Randomize(Color*& cols, int count)
{
    for (int c=0; c<count; c++)
    {
        uint32_t rn = Random::GetUint();
        cols[c] = *reinterpret_cast<Color*>(&rn);
        //cols[c] = {200,0,0,255};
    }
}
