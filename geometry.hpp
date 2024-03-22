#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <glm/glm.hpp>
#include "model.hpp"
#include "ray.hpp"

struct BoundingBox
{
    glm::vec3 vMin, vMax;

    BoundingBox() : vMin(std::numeric_limits<float>::max()), vMax(std::numeric_limits<float>::min()) {}
};

class Geometry
{
public:
    virtual BoundingBox getBoundingBox(const std::vector<Model::Vertex> &vertices);
    virtual HitResult getIntersection(const Ray &ray);
};

class Triangle : public Geometry
{
public:
    Triangle(uint32_t _v0, uint32_t _v1, uint32_t _v2)
        : v0(_v0), v1(_v1), v2(_v2) {}

    // 重载 [] 操作符
    uint32_t operator[](size_t index) const
    {
        assert(index < 3 && "out of range!");
        if (index == 0)
        {
            return v0;
        }
        else if (index == 1)
        {
            return v1;
        }
        else
        {
            return v2;
        }
    }

public:
    BoundingBox getBoundingBox(const std::vector<Model::Vertex> &vertices) override;
    HitResult getIntersection(const Ray &ray) override;

private:
    // 三角形对应顶点在indices中的下标
    uint32_t v0, v1, v2;
};
#endif // __GEOMETRY_H__