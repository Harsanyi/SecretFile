#pragma once
#include "raylib.h"

inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
    return {a.x-b.x, a.y-b.y};
}

template<typename T>
inline bool Contains(const std::vector<T>& container, const T& element)
{
    return std::count(container.begin(), container.end(), element) > 0;
}

inline Rectangle FitSizeInto(int width, int height, Rectangle rect)
{
    if (width > rect.width)
    {
        height = (int)(height*(rect.width / (float)width));
        width = rect.width;
    }

    if (height > rect.height)
    {
        width = (int)(width*(rect.height / (float)height));
        height = rect.height;
    }

    Vector2 center = {rect.x + rect.width / 2, rect.y + rect.height / 2};
    return Rectangle{center.x - width/2, center.y - height/2, (float)width, (float)height};
}

inline long ByteCount(const Texture2D& tex)
{
    return tex.width * tex.height * 4;
}

inline long ByteCount(const Image& img)
{
    return img.width * img.height * 4;
}

inline long ByteCount(const Wave& wave)
{
    return wave.frameCount * wave.channels * (wave.sampleSize / 8);
}
