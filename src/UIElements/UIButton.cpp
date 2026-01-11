#include "UIButton.h"

#include "raygui.h"

void UIButton::Draw()
{
    if (GuiButton(rect, label.c_str()))
    {
        onClick.Invoke();
    }
}
