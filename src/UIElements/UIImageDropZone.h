#pragma once
#include "UIDropZone.h"

class UIImageDropZone : public UIDropZone
{
private:
    const std::vector<std::string> FORMATS{".jpg",".png"};
    
    Texture2D droppedTexture;
    bool haveTexture = false;

    void OnFileDropped(char* path);

public:
    UIImageDropZone();
    
    Event<Texture2D*> onImageDropped;
    
    void Draw() override;
};
