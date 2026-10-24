#include "Obj.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <stdexcept>

void Obj::SaveToFile(const Model& model, const std::string& filename)
{
    if (model.meshCount < 1 || !model.meshes || !model.meshes[0].vertices)
        throw std::invalid_argument("OBJ requires a first mesh with vertex positions");
    const Mesh& mesh = model.meshes[0];
    for (int i = 0; i < mesh.vertexCount * 3; ++i)
        if (!std::isfinite(mesh.vertices[i])) throw std::runtime_error("OBJ cannot preserve non-finite positions");
    for (int i = 0; mesh.indices && i < mesh.triangleCount * 3; ++i)
        if (mesh.indices[i] >= mesh.vertexCount) throw std::out_of_range("Invalid mesh vertex index");
    if (!mesh.indices && mesh.triangleCount * 3 > mesh.vertexCount)
        throw std::out_of_range("Invalid triangle-expanded mesh");

    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Cannot open OBJ output");
    file.imbue(std::locale::classic());
    file << std::setprecision(std::numeric_limits<float>::max_digits10);
    for (int i = 0; i < mesh.vertexCount; ++i)
        file << "v " << mesh.vertices[i * 3] << ' ' << mesh.vertices[i * 3 + 1] << ' ' << mesh.vertices[i * 3 + 2] << '\n';
    if (mesh.normals)
        for (int i = 0; i < mesh.vertexCount; ++i)
            file << "vn " << mesh.normals[i * 3] << ' ' << mesh.normals[i * 3 + 1] << ' ' << mesh.normals[i * 3 + 2] << '\n';
    if (mesh.texcoords)
        for (int i = 0; i < mesh.vertexCount; ++i)
            file << "vt " << mesh.texcoords[i * 2] << ' ' << mesh.texcoords[i * 2 + 1] << '\n';
    for (int triangle = 0; triangle < mesh.triangleCount; ++triangle)
    {
        file << 'f';
        for (int corner = 0; corner < 3; ++corner)
        {
            const int offset = triangle * 3 + corner;
            const unsigned index = (mesh.indices ? mesh.indices[offset] : offset) + 1u;
            file << ' ' << index;
            if (mesh.texcoords || mesh.normals)
            {
                file << '/';
                if (mesh.texcoords) file << index;
                if (mesh.normals) file << '/' << index;
            }
        }
        file << '\n';
    }
    file.close();
    if (!file) throw std::runtime_error("OBJ output could not be completed");
}
