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
