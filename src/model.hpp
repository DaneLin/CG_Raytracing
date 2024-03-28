#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.hpp"
#include "material.hpp"

// thirdparty
#include <glm/glm.hpp>

#include <tiny_obj_loader.h>

// std
#include <string>
#include <vector>
#include <memory>

class Model
{
public:
    Model() = default;
    Model(const std::string &modelPath);

    ~Model() = default;

    uint32_t getIndicesCount() { return indices.size(); }
    void getTriangles(std::vector<Triangle> &triangles);
    std::vector<std::shared_ptr<Material>> &getMaterials() { return materials; }

private:
    void loadModel(const std::string &modelPath);
    void loadMaterials();

private:
    // 使用tinyobjloader提供的读取模板
    tinyobj::ObjReaderConfig readerConfig;
    tinyobj::ObjReader reader;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::shared_ptr<Material>> materials;
};
#endif // __MODEL_H__