#include "UIImage.h"

void UIImage::Draw()
{
    DrawTexturePro(texture, Rectangle{0,0,(float)texture.width, (float)texture.height}, transform.GetRect(), Vector2{0.5,0.5}, 0., color);
}
