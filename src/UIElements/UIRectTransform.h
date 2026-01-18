#pragma once
#include <vector>
#include "raylib.h"

class UIRectTransform
{
private:
    Rectangle rect{0.,0.,100.,100.};
    Vector2 pivot{0.,0.};

    std::vector<UIRectTransform*> childs = std::vector<UIRectTransform*>(0);
    UIRectTransform* parent = nullptr;

public:
    Rectangle GetLocalRect() const;
    Rectangle GetGlobalRect() const;
    
    void SetPosition(const Vector2& locPosition);
    Vector2 GetPosition() const;
    void SetPivot(const Vector2& locPivot);
    void SetSize(const Vector2& locSize);
    Vector2 GetSize() const;

    int ChildCount() const;
    UIRectTransform* GetChildAt(int i)const;
    void RemoveChild(UIRectTransform* child);
    void AddChild(UIRectTransform* child);
    bool IsChild(UIRectTransform* child);
    void SetParent(UIRectTransform* parent);
};
