#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

// std
#include <memory>

#include <glm/glm.hpp>
#include "ray.hpp"
#include "record.hpp"

const float EPSILON = 0.0000001f;
const float OFFSET = 0.001f;

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    int faceMaterialID = -1;
    bool operator==(const Vertex &v) const
    {
        return position == v.position &&
               color == v.color &&
               normal == v.normal &&
               uv == v.uv;
    }
};

const float PADDING = 0.01f;

struct BoundingBox
{
    glm::vec3 vMin, vMax;

    BoundingBox() : vMin(std::numeric_limits<float>::max()), vMax(std::numeric_limits<float>::min()) {}
    void merge(const glm::vec3 &other)
    {
        vMin.x = std::min(vMin.x, other.x);
        vMin.y = std::min(vMin.y, other.y);
        vMin.z = std::min(vMin.z, other.z);
        vMax.x = std::max(vMax.x, other.x);
        vMax.y = std::max(vMax.y, other.y);
        vMax.z = std::max(vMax.z, other.z);

        for (int i = 0; i < 3; i++)
        {
            if (vMax[i] - vMin[i] <= PADDING)
            {
                vMax[i] += PADDING;
                vMin[i] -= PADDING;
            }
        }
    }

    bool intersect(const Ray &ray) const
    {
        float tmin = (vMin.x - ray.origin.x) / ray.direction.x;
        float tmax = (vMax.x - ray.origin.x) / ray.direction.x;

        if (tmin > tmax)
            std::swap(tmin, tmax);

        float tymin = (vMin.y - ray.origin.y) / ray.direction.y;
        float tymax = (vMax.y - ray.origin.y) / ray.direction.y;

        if (tymin > tymax)
            std::swap(tymin, tymax);

        if ((tmin > tymax) || (tmax < tymin))
            return false;
        return true;
    }
};

class Geometry
{
public:
    virtual BoundingBox getBoundingBox();
    virtual HitResult getIntersection(const Ray &ray);
};

class Triangle : public Geometry
{
public:
    Triangle() = default;
    Triangle(Vertex _v0, Vertex _v1, Vertex _v2)
        : v0(_v0), v1(_v1), v2(_v2) {}

    // 重载 [] 操作符
    Vertex operator[](size_t index) const
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
    BoundingBox getBoundingBox() override;
    HitResult getIntersection(const Ray &ray) override;

private:
    Vertex v0{}, v1{}, v2{};
};
#endif // __GEOMETRY_H__