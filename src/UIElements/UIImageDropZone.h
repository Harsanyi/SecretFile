#pragma once
#include "UIDropZone.h"

class UIImageDropZone : public UIDropZone
{
private:
    const std::vector<std::string> FORMATS{".jpg",".png"};
    
    Texture2D* texture;
    bool haveTexture = false;

    void OnFileDropped(char* path);

public:
    UIImageDropZone();
    
    Event<Texture2D*> onImageDropped;
    
    void Draw() override;
    void SetTexture(Texture2D* texture);
};
