#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

// std
#include <memory>

#include <glm/glm.hpp>
#include "ray.hpp"
#include "record.hpp"
#include "helper.hpp"
#include "material.hpp"

const float EPSILON = 0.0000001f;
const float OFFSET = 0.0000001f;
const float PADDING = 0.01f;

struct BoundingBox
{
    glm::vec3 vMin, vMax;

    BoundingBox() : vMin(std::numeric_limits<float>::max()), vMax(std::numeric_limits<float>::min()) {}

    glm::vec3 centroid() { return 0.5f * vMin + 0.5f * vMax; }
    void merge(const glm::vec3 &other)
    {
        vMin.x = std::min(vMin.x, other.x);
        vMin.y = std::min(vMin.y, other.y);
        vMin.z = std::min(vMin.z, other.z);
        vMax.x = std::max(vMax.x, other.x);
        vMax.y = std::max(vMax.y, other.y);
        vMax.z = std::max(vMax.z, other.z);
        auto split = PADDING / 2;
        for (int i = 0; i < 3; i++)
        {
            if (vMax[i] - vMin[i] <= PADDING)
            {
                vMax[i] += split;
                vMin[i] -= split;
            }
        }
    }

    int maxAxis()
    {
        glm::vec3 minus = vMax - vMin;
        if (minus.x > minus.y && minus.x > minus.z)
            return 0;
        else if (minus.y > minus.z)
            return 1;
        else
            return 2;
    }

    bool intersect(const Ray &ray) const
    {
        float tmin = 0.00000f;
        float tmax = std::numeric_limits<float>().max();
        glm::vec3 inv = 1.f / ray.direction;
        for (int i = 0; i < 3; i++)
        {
            float minTime = (vMin[i] - ray.origin[i]) * inv[i];
            float maxTime = (vMax[i] - ray.origin[i]) * inv[i];
            if (minTime > maxTime)
                std::swap(minTime, maxTime);

            tmin = std::max(tmin, minTime);
            tmax = std::min(tmax, maxTime);

            if (tmin > tmax)
                return false;
        }
        return tmax >= 0;
    }
};

inline BoundingBox unionBox(const BoundingBox &bb1, const BoundingBox &bb2)
{
    BoundingBox ret;
    ret.vMin = glm::min(bb1.vMin, bb2.vMin);
    ret.vMax = glm::max(bb1.vMax, bb2.vMax);
    return ret;
}

inline BoundingBox unionBox(const BoundingBox &bb1, const glm::vec3 &other)
{
    BoundingBox ret;
    ret.vMin = glm::min(bb1.vMin, other);
    ret.vMax = glm::max(bb1.vMax, other);
    return ret;
}

class Geometry
{
public:
    virtual BoundingBox getBoundingBox() = 0;
    virtual HitResult getIntersection(const Ray &ray) = 0;
    virtual float getArea() = 0;
    virtual void sample(HitResult &point, float &pdf) = 0;
};

class Triangle : public Geometry
{
public:
    glm::vec3 v0, v1, v2;
    glm::vec3 edge1, edge2; // v1 - v0, v2 - v0;
    glm::vec2 uv0, uv1, uv2;
    glm::vec3 faceNormal;
    float areaSize;
    std::shared_ptr<Material> mat;

public:
    Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2);

    void setUVs(glm::vec2 _uv0, glm::vec2 _uv1, glm::vec2 _uv2);
    float getArea() override;
    BoundingBox getBoundingBox() override;
    HitResult getIntersection(const Ray &ray) override;
    void sample(HitResult &point, float &pdf) override;
};
#endif // __GEOMETRY_H__