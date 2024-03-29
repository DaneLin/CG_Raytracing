#include "model.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
// std
#include <iostream>
#include <unordered_map>
#include <functional>
#include <cstring>

const std::string bathTexturesPath = "../models/bathroom/textures";

Model::Model(const std::string &modelPath, const std::vector<glm::vec3> &radiances)
{
    loadModel(modelPath, radiances);
}

void Model::loadModel(const std::string &modelPath, const std::vector<glm::vec3> &radiances)
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

    auto &mats = reader.GetMaterials();
    std::cout << "Materials: " << mats.size() << '\n';
    materials.clear();
    materials.resize(mats.size());
    std::vector<bool> isLights(mats.size(), false);
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
                materials[i] = std::make_shared<LightSource>(radiances[idx - 1]);
                isLights[i] = true;
                std::cout << "made a light source material!" << '\n';
            }
            else if (std::strncmp(mats[i].name.c_str(), "Mirror", 6) == 0)
            {
                materials[i] = std::make_shared<Mirror>();
                std::cout << "made a Mirorr material!" << '\n';
            }
            else if (mats[i].specular[0] != 0)
            {
                // std::cout << "Shininess: " << mats[i].shininess << '\n';
                materials[i] = std::make_shared<GlossyMaterial>(kd, 1.f / mats[i].shininess);
                std::cout << "made a Glossy material!" << '\n';
            }
            else if (mats[i].diffuse_texname != "")
            {
                materials[i] = std::make_shared<LambertianTextured>(std::make_shared<Texture>(bathTexturesPath + mats[i].diffuse_texname));
                std::cout << "made a Lambertian material with texture!" << '\n';
            }
            else
            {
                materials[i] = std::make_shared<Lambertian>(kd);
                std::cout << "made a solid color material!" << '\n';
            }
        }
    }

    // loop over shapes
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            // Get material ID for this face
            int matID = shapes[s].mesh.material_ids[f];
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            Vertex vertices[3];
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];

                Vertex vertex{};
                if (idx.vertex_index >= 0)
                {
                    vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
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

                vertices[v] = vertex;
            }
            Triangle tris(vertices[0].position, vertices[1].position, vertices[2].position);
            tris.setUVs(vertices[0].uv, vertices[1].uv, vertices[2].uv);
            tris.mat = materials[matID];
            triangles.emplace_back(tris);
            if (isLights[matID])
                lights.emplace_back(tris);
            indexOffset += fv;
        }
    }
}

std::vector<Triangle> &Model::getTriangles()
{
    return triangles;
}
