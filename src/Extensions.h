#pragma once
#include "raylib.h"
#include "rlgl.h"
#include "Capacity.h"
#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <vector>

inline Rectangle FitSizeInto(int width, int height, Rectangle rect)
{
    if (width <= 0 || height <= 0) return {rect.x, rect.y, 0, 0};
    const float scale = std::min({rect.width / width, rect.height / height, 1.0f});
    const float w = width * scale, h = height * scale;
    return {rect.x + (rect.width - w) / 2, rect.y + (rect.height - h) / 2, w, h};
}

inline uint64_t ByteCount(const Texture2D& texture)
{
    return CheckedMultiply(CheckedMultiply(texture.width, texture.height), 4);
}
inline uint64_t ByteCount(const Image& image)
{
    return CheckedMultiply(CheckedMultiply(image.width, image.height), 4);
}
inline uint64_t ByteCount(const Wave& wave)
{
    return CheckedMultiply(CheckedMultiply(wave.frameCount, wave.channels), wave.sampleSize / 8);
}
inline uint64_t ByteCount(const Model& model)
{
    if (model.meshCount < 1 || !model.meshes || model.meshes[0].vertexCount <= 0 || !model.meshes[0].vertices)
        throw std::invalid_argument("Model has no first-mesh vertex positions");
    return CheckedMultiply(model.meshes[0].vertexCount, 3 * sizeof(float));
}

inline void* AllocateBytes(uint64_t size)
{
    if (size > std::numeric_limits<unsigned int>::max())
        throw std::length_error("Resource exceeds raylib allocation limit");
    void* memory = MemAlloc(static_cast<unsigned int>(size));
    if (!memory && size != 0) throw std::bad_alloc();
    return memory;
}

inline void UnloadOwnedModel(Model& model)
{
    std::unordered_set<unsigned int> textures;
    for (int m = 0; m < model.materialCount; ++m)
        if (model.materials[m].maps)
            for (int i = 0; i <= MATERIAL_MAP_BRDF; ++i)
            {
                const auto texture = model.materials[m].maps[i].texture;
                if (texture.id && texture.id != rlGetTextureIdDefault() && textures.insert(texture.id).second)
                    UnloadTexture(texture);
            }
    UnloadModel(model);
}

struct ModelDeleter
{
    void operator()(Model* model) const
    {
        if (model) { UnloadOwnedModel(*model); delete model; }
    }
};
using OwnedModel = std::unique_ptr<Model, ModelDeleter>;

inline Model* CloneModel(const Model& model)
{
    if (model.meshCount < 1 || !model.meshes) throw std::invalid_argument("Cannot clone an empty model");
    OwnedModel clone(new Model{});
    clone->transform = model.transform;
    // Geometry-only clone: default material maps are independently allocated; no source resources are shared.
    clone->materials = static_cast<Material*>(AllocateBytes(sizeof(Material)));
    clone->materials[0] = LoadMaterialDefault();
    clone->materialCount = 1;
    clone->meshMaterial = static_cast<int*>(AllocateBytes(sizeof(int) * model.meshCount));
    std::memset(clone->meshMaterial, 0, sizeof(int) * model.meshCount);
    clone->meshes = static_cast<Mesh*>(AllocateBytes(sizeof(Mesh) * model.meshCount));
    std::memset(clone->meshes, 0, sizeof(Mesh) * model.meshCount);
    for (int m = 0; m < model.meshCount; ++m)
    {
        const Mesh& source = model.meshes[m];
        Mesh& mesh = clone->meshes[m];
        ++clone->meshCount;
        mesh.vertexCount = source.vertexCount;
        mesh.triangleCount = source.triangleCount;
        const auto copy = [](auto* pointer, uint64_t count)
        {
            using T = std::remove_pointer_t<decltype(pointer)>;
            if (!pointer) return static_cast<T*>(nullptr);
            const auto size = CheckedMultiply(count, sizeof(T));
            auto* result = static_cast<T*>(AllocateBytes(size));
            std::memcpy(result, pointer, static_cast<std::size_t>(size));
            return result;
        };
        mesh.vertices = copy(source.vertices, CheckedMultiply(source.vertexCount, 3));
        mesh.normals = copy(source.normals, CheckedMultiply(source.vertexCount, 3));
        mesh.texcoords = copy(source.texcoords, CheckedMultiply(source.vertexCount, 2));
        mesh.indices = copy(source.indices, CheckedMultiply(source.triangleCount, 3));
        UploadMesh(&mesh, true);
    }
    return clone.release();
}

inline std::vector<uint8_t> GetModelVertData(const Model& model)
{
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559, "Requires IEEE-754 binary32");
    const auto size = ByteCount(model);
    const auto* begin = reinterpret_cast<const uint8_t*>(model.meshes[0].vertices);
    return std::vector<uint8_t>(begin, begin + static_cast<std::size_t>(size));
}

inline void SetModelVertData(Model& model, const std::vector<uint8_t>& data)
{
    if (data.size() != ByteCount(model)) throw std::invalid_argument("Vertex buffer size mismatch");
    std::memcpy(model.meshes[0].vertices, data.data(), data.size());
    if (model.meshes[0].vboId)
    {
        if (data.size() > std::numeric_limits<int>::max()) throw std::length_error("Vertex buffer is too large");
        UpdateMeshBuffer(model.meshes[0], 0, model.meshes[0].vertices, static_cast<int>(data.size()), 0);
    }
}
