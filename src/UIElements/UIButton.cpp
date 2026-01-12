#include "UIButton.h"

#include "raygui.h"

void UIButton::Draw()
{
    if (GuiButton(transform.GetRect(), label.c_str()))
    {
        onClick.Invoke();
    }
}
