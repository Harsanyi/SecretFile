#include "UIElement.h"

#include <iostream>

std::vector<UIElement*> UIElement::instances;

UIElement::UIElement()
{
    instances.push_back(this);
}

UIElement::~UIElement()
{
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void UIElement::DrawAll()
{
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i] != nullptr)instances[i]->Draw();
    }
}
