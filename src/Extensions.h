#pragma once
#include "raylib.h"

template<typename T>
inline bool Contains(const std::vector<T>& container, const T& element)
{
    return std::count(container.begin(), container.end(), element) > 0;
}

inline Rectangle FitSizeInto(int width, int height, Rectangle rect)
{
    if (width > rect.width)
    {
        height = (int)(height*(rect.width / (float)width));
        width = rect.width;
    }

    if (height > rect.height)
    {
        width = (int)(width*(rect.height / (float)height));
        height = rect.height;
    }

    Vector2 center = {rect.x + rect.width / 2, rect.y + rect.height / 2};
    return Rectangle{center.x - width/2, center.y - height/2, (float)width, (float)height};
}

inline long ByteCount(const Texture2D& tex)
{
    return tex.width * tex.height * 4;
}

inline long ByteCount(const Image& img)
{
    return img.width * img.height * 4;
}

inline long ByteCount(const Wave& wave)
{
    return wave.frameCount * wave.channels * (wave.sampleSize / 8);
}

inline Model CloneModel(const Model& model)
{
    Model clone = model;
    Mesh& mesh = clone.meshes[0];
    
    long vertexDatasize = mesh.vertexCount * sizeof(float) * 3;
    float* vertices = (float*)MemAlloc(vertexDatasize);
    memcpy(vertices, model.meshes[0].vertices, vertexDatasize);
    mesh.vertices = vertices;
    
    UpdateMeshBuffer(mesh, 0, vertices, vertexDatasize, 0);
    return clone;
}

inline std::vector<float> GetModelVertices(const Model& model)
{
    long size = model.meshes[0].vertexCount * 3;
    float* vertPtr = model.meshes[0].vertices;
    std::vector<float> vertices(vertPtr, vertPtr + size);
    return vertices;
}

inline void SetModelVertices(Model& model, std::vector<float>& vertices)
{
    model.meshes[0].vertexCount = vertices.size()/3;
    model.meshes[0].vertices = vertices.data();
    UpdateMeshBuffer(model.meshes[0], 0, vertices.data(), vertices.size() * sizeof(float) * 3, 0);
}
