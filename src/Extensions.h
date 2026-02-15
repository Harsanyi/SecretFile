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

inline long ByteCount(const Model& model)
{
    return model.meshes[0].vertexCount * 3 * 4;
}

inline Model* CloneModel(const Model& model)
{
    Model* clone = new Model();
    clone->transform = model.transform;

    clone->materialCount = model.materialCount;
    clone->materials = (Material*)MemAlloc(sizeof(Material) * model.materialCount);
    for (int m=0; m<model.materialCount; m++)
    {
        clone->materials[m] = model.materials[m];
    }

    clone->meshMaterial = (int*)MemAlloc(sizeof(int)*model.meshCount);
    memcpy(clone->meshMaterial, model.meshMaterial, sizeof(int)*model.meshCount);

    clone->meshCount = model.meshCount;
    clone->meshes = (Mesh*)MemAlloc(sizeof(Mesh) * model.meshCount);
    for (int m=0; m<model.meshCount; m++)
    {
        Mesh modelMesh = model.meshes[m];
        Mesh mesh = {0};

        mesh.vertexCount = modelMesh.vertexCount;
        mesh.vertices = (float*)MemAlloc(sizeof(float)*3*modelMesh.vertexCount);
        memcpy(mesh.vertices, modelMesh.vertices, sizeof(float)*3*modelMesh.vertexCount);

        mesh.normals = (float*)MemAlloc(sizeof(float)*3*modelMesh.vertexCount);
        memcpy(mesh.normals, modelMesh.normals, sizeof(float)*3*modelMesh.vertexCount);

        mesh.texcoords = (float*)MemAlloc(sizeof(float)*2*modelMesh.vertexCount);
        memcpy(mesh.texcoords, modelMesh.texcoords, sizeof(float)*2*modelMesh.vertexCount);
        
        mesh.triangleCount = modelMesh.triangleCount;
        if (mesh.indices != nullptr)
        {
            mesh.indices = (unsigned short*)MemAlloc(modelMesh.triangleCount*3);
            memcpy(mesh.indices, modelMesh.indices, modelMesh.triangleCount*3);
        }else
        {
            mesh.indices = nullptr;
        }

        mesh.animNormals = nullptr;
        mesh.animVertices = nullptr;
        mesh.boneCount = 0;
        mesh.boneMatrices = nullptr;
        mesh.boneIds = nullptr;
        mesh.colors = nullptr;
        mesh.boneWeights = nullptr;
        mesh.texcoords2 = nullptr;

        clone->meshes[m] = mesh;
        
        UploadMesh(&clone->meshes[m],false);
    }
    
    return clone;
}

inline std::vector<uint8_t> GetModelVertData(const Model& model)
{
    long byteCount = model.meshes[0].vertexCount * 3 * 4;
    uint8_t* vertPtr = (uint8_t*)model.meshes[0].vertices;
    std::vector<uint8_t> vertices(byteCount);
    memcpy(vertices.data(), vertPtr, byteCount);
    return vertices;
}

inline void SetModelVertData(Model& model, std::vector<uint8_t>& data)
{
    model.meshes[0].vertexCount = data.size()/(3*4);
    model.meshes[0].vertices = (float*)MemAlloc(data.size());
    memcpy(model.meshes[0].vertices, data.data(), data.size());
    
    UpdateMeshBuffer(model.meshes[0], 0, model.meshes[0].vertices, data.size(), 0);
}
