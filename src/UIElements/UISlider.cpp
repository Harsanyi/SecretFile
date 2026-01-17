#include "UISlider.h"

#include <iomanip>
#include <ios>
#include <sstream>

#include "raygui.h"

void UISlider::Draw()
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(intValues ? 0 : 2) << value;
    float oldValue = value;
    GuiSlider(transform.GetGlobalRect(), label.c_str(), ss.str().c_str(), &value, min, max);
    if (intValues)
    {
        value = std::round(value);
    }

    if (oldValue != value)
    {
        onValueChanged.Invoke(value);
    }
}
