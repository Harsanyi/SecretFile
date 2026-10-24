#pragma once
#include <string>
#include "raylib.h"

class Obj
{
public:
    static void SaveToFile(const Model& model, const std::string& filename);
};
