#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.hpp"
#include "material.hpp"

// thirdparty
#include <glm/glm.hpp>

// std
#include <string>
#include <vector>
#include <memory>

class Model
{
public:
    Model() = default;
    Model(const std::string &modelPath, const std::vector<glm::vec3> &radiances);

    ~Model() = default;

    uint32_t getIndicesCount() { return static_cast<uint32_t>(indices.size()); }
    void getTriangles(std::vector<Triangle> &triangles);
    std::vector<std::shared_ptr<Material>> &getMaterials() { return materials; }
    std::vector<AreaLight> &getLights() { return lights; }

private:
    void loadModel(const std::string &modelPath, const std::vector<glm::vec3> &radiances);

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<AreaLight> lights;
};
#endif // __MODEL_H__