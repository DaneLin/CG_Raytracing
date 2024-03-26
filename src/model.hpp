#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.hpp"

// thirdparty
#include <glm/glm.hpp>

// std
#include <string>
#include <vector>

class Model
{
public:
    Model() = default;
    Model(const std::string &modelPath);

    ~Model() = default;

    uint32_t getIndicesCount() { return indices.size(); }
    void getTriangles(std::vector<Triangle> &triangles);

private:
    void loadModel(const std::string &modelPath);

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
#endif // __MODEL_H__