#include "UIButton.h"

#include "raygui.h"

UIButton::UIButton(){}

UIButton::UIButton(Rectangle rect, char* label) : rect(rect), label(label){}

void UIButton::Draw()
{
    if (GuiButton(rect, label))
    {
        onClick.Invoke();
    }
}
