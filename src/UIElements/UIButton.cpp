#include "UIButton.h"

#include "raygui.h"

void UIButton::Draw()
{
    if (GuiButton(transform.GetGlobalRect(), label.c_str()))
    {
        onClick.Invoke();
    }
}
