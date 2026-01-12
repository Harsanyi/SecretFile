#pragma once
#include <string>

#include "Event.h"
#include "UIElement.h"
#include "UIImage.h"

class UIDropZone : public UIElement
{
private:
    static Texture2D dropTexture;

    UIImage dropImage;
    
public:
    Event<std::string> onFileDropped;

    UIDropZone();
    
    void Draw() override;
};
