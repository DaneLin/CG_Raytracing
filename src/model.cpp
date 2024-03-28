#include "model.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
// std
#include <iostream>
#include <unordered_map>
#include <functional>
#include <cstring>

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >>
                    1) ^
                   (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}

Model::Model(const std::string &modelPath)
{
    loadModel(modelPath);
}

void Model::loadModel(const std::string &modelPath)
{
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

    vertices.clear();
    indices.clear();
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    // loop over shapes
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            // Get material ID for this face
            int matID = shapes[s].mesh.material_ids[f];

            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];

                Vertex vertex{};
                vertex.faceMaterialID = matID;
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
    std::cout << "Indices: " << indices.size() << std::endl;
}

void Model::loadMaterials(const std::vector<glm::vec3> &lights)
{
    auto &mats = reader.GetMaterials();
    std::cout << "Materials: " << mats.size() << '\n';
    materials.clear();
    materials.resize(mats.size());
    for (size_t i = 0; i < mats.size(); ++i)
    {
        std::cout << "material " << i << ": " << mats[i].name << std::endl;
        std::cout << "  diffuse_texname: " << mats[i].diffuse_texname << std::endl;
        std::cout << "  diffuse color: " << mats[i].diffuse[0] << ' '
                  << mats[i].diffuse[1] << ' '
                  << mats[i].diffuse[2] << std::endl;
        glm::vec3 kd = {mats[i].diffuse[0], mats[i].diffuse[1], mats[i].diffuse[2]};
        // solid color
        if (mats[i].diffuse_texname.length() == 0)
        {
            if (std::strncmp(mats[i].name.c_str(), "light", 5) == 0 || std::strncmp(mats[i].name.c_str(), "Light", 5) == 0)
            {
                int idx = 1;
                if (std::strlen(mats[i].name.c_str()) > 5)
                {
                    idx = mats[i].name.back() - '0';
                }

                materials[i] = std::make_shared<LightSource>(lights[idx - 1]);
                std::cout << "made a light source material!" << '\n';
            }
            else if (std::strncmp(mats[i].name.c_str(), "Mirror", 6) == 0)
            {
                materials[i] = std::make_shared<Mirror>();
                std::cout << "made a Miroor material!" << '\n';
            }
            else
            {
                materials[i] = std::make_shared<Lambertian>(kd);
                std::cout << "made a solid color material!" << '\n';
            }
        }
    }
}

void Model::getTriangles(std::vector<Triangle> &triangles)
{
    triangles.clear();

    for (uint32_t idx = 0; idx < indices.size(); idx += 3)
    {
        Triangle tmp{vertices[indices[idx]], vertices[indices[idx + 1]], vertices[indices[idx + 2]]};
        triangles.push_back(tmp);
    }
}
