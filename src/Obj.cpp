#include "Obj.h"

#include <fstream>
#include <iomanip>
#include <iostream>

void Obj::SaveToFile(Model& model, std::string filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        TraceLog(LOG_ERROR, "Failed to open file %s for writing", filename.c_str());
        return;
    }

    Mesh& mesh = model.meshes[0];
    file << std::setprecision(std::numeric_limits<float>::max_digits10);
    
    WriteVXList(file,"v", mesh.vertices, mesh.vertexCount, 3);
    WriteVXList(file, "vn", mesh.normals, mesh.vertexCount, 3);

    if (mesh.indices != nullptr)
    {
        std::cout << "Have indicies!\n";
        for (int t=0; t<mesh.triangleCount; t++)
        {
            int i=t*3;
            file << "f ";
            file << mesh.indices[i+1] << "//" << mesh.indices[i+1] << " ";
            file << mesh.indices[i+1+1] << "//" << mesh.indices[i+1+1] << " ";
            file << mesh.indices[i+2+1] << "//" << mesh.indices[i+2+1] << "\n";
        }
    }else
    {
        std::cout << "There is no indicies!\n";
        for (int v=0; v<mesh.vertexCount; v+=3)
        {
            file << "f ";
            file << v+1 << "//" << v+1 << " ";
            file << v+1+1 << "//" << v+1+1 << " ";
            file << v+2+1 << "//" << v+2+1 << "\n";
        }   
    }
    
    file.close();
}

void Obj::WriteVXList(std::ofstream& file, std::string prefix, float* vXValues, long vXCount, int x)
{
    for (int i=0; i < vXCount; i++)
    {
        int v = i*x;
        file << prefix << " ";
        for (int n=0; n < x; n++)
        {
            file << vXValues[v+n] << " ";
        }
        file << "\n";
    }
}
