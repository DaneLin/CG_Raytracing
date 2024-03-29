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

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 uv{};
    bool operator==(const Vertex &v) const
    {
        return position == v.position &&
               normal == v.normal &&
               uv == v.uv;
    }
};

class Model
{
public:
    Model(const std::string &modelPath, const std::vector<glm::vec3> &radiances);

    ~Model() = default;
    std::vector<Triangle> &getTriangles();
    std::vector<std::shared_ptr<Material>> &getMaterials() { return materials; }
    std::vector<Triangle> &getLights() { return lights; }

private:
    void loadModel(const std::string &modelPath, const std::vector<glm::vec3> &radiances);

private:
    std::vector<Triangle> triangles;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<Triangle> lights;
};
#endif // __MODEL_H__