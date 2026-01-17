#pragma once
#include <string>

#include "Event.h"
#include "UIElement.h"
#include "UIImage.h"

class UIDropZone : public UIElement
{
protected:
    static Texture2D dropTexture;

    UIImage dropImage;
    
public:
    Event<char*> onFileDropped;

    UIDropZone();
    
    void Draw() override;
    void CheckFileDropped();
};
