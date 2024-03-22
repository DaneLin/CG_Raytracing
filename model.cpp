#include "model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <loader/tiny_obj_loader.h>

#include <glm/gtx/hash.hpp>

// std
#include <iostream>
#include <unordered_map>
#include <functional>

namespace std
{
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    inline void hashCombine(std::size_t &seed, T const &v, Rest &&...rest)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (int[]){0, (hashCombine(seed, std::forward<Rest>(rest)), 0)...};
    }
    template <>
    struct hash<Model::Vertex>
    {
        size_t operator()(Model::Vertex const &vertex) const noexcept
        {
            size_t seed = 0;
            hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

Model::Model(const std::string &modelPath)
{
    loadModel(modelPath);
}

void Model::loadModel(const std::string &modelPath)
{
    // 使用tinyobjloader提供的读取模板
    tinyobj::ObjReaderConfig readerConfig;
    tinyobj::ObjReader reader;
    readerConfig.mtl_search_path = "";

    if (!reader.ParseFromFile(modelPath, readerConfig))
    {
        if (!reader.Error().empty())
        {
            throw std::runtime_error(reader.Error());
        }
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning() << '\n';
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();
    std::cout << "Materials: " << materials.size() << '\n';
    for (size_t i = 0; i < materials.size(); ++i)
    {
        std::cout << "material " << i << ": " << materials[i].name << std::endl;
        std::cout << "  diffuse_texname: " << materials[i].diffuse_texname << std::endl;
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    // loop over shapes
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
                Vertex vertex{};
                if (idx.vertex_index >= 0)
                {
                    vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    vertex.color.x = attrib.colors[3 * size_t(idx.vertex_index) + 0];
                    vertex.color.y = attrib.colors[3 * size_t(idx.vertex_index) + 1];
                    vertex.color.z = attrib.colors[3 * size_t(idx.vertex_index) + 2];
                }

                // Check for normal
                if (idx.normal_index >= 0)
                {

                    vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                if (idx.texcoord_index >= 0)
                {
                    vertex.uv.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.uv.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.emplace_back(vertex);
                }
                indices.emplace_back(uniqueVertices[vertex]);
            }
            indexOffset += fv;
        }
    }
}
Model::Vertex &Model::getVertex(uint32_t idx)
{
    return vertices[idx];
}
