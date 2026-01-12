#include "UIRectTransform.h"

Rectangle UIRectTransform::GetRect()
{
    Vector2 min = {position.x - pivot.x * size.x, position.y - pivot.y * size.y};
    return Rectangle(min.x, min.y, size.x, size.y);
}
