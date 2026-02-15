#pragma once
#include <string>

#include "raylib.h"

class Obj
{
public:
    static void SaveToFile(Model& model, std::string filename);
    static void WriteVXList(std::ofstream& file,std::string prefix, float* vXValues, long vXCount, int x);
};
