#ifndef __MODEL_H__
#define __MODEL_H__

#include <glm/glm.hpp>

// std
#include <string>
#include <vector>

class Model
{
public:
    struct Vertex
    {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        bool operator==(const Vertex &v) const
        {
            return position == v.position &&
                   color == v.color &&
                   normal == v.normal &&
                   uv == v.uv;
        }
    };

    Model() = default;
    Model(const std::string &modelPath);

    ~Model() = default;

    uint32_t getIndicesCount() { return indices.size(); }
    Model::Vertex &getVertex(uint32_t idx);

private:
    void loadModel(const std::string &modelPath);

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
#endif // __MODEL_H__