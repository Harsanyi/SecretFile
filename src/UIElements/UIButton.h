#pragma once
#include "Event.h"
#include "UIDrawable.h"

struct UIButton : UIDrawable
{
protected:
    ~UIButton() = default;

public:
    char* label = (char*)"Button";
    Event<> onClick;
    Rectangle rect{10,10,100,25};

    UIButton();
    UIButton(Rectangle rect, char* label);
    
    void Draw() override;
};
