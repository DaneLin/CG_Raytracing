#ifndef __BVH_H__
#define __BVH_H__

#include "geometry.hpp"
#include "ray.hpp"

// std
#include <vector>

struct BVHNode
{
    BoundingBox boundingBox;

    BVHNode *leftNode;
    BVHNode *rightNode;
    std::vector<Triangle> triangles;

    BVHNode(const std::vector<Triangle> &tris)
        : triangles(tris), leftNode(nullptr), rightNode(nullptr)
    {
        for (auto &triangle : triangles)
        {
            boundingBox.merge(triangle.v0);
            boundingBox.merge(triangle.v1);
            boundingBox.merge(triangle.v2);
        }
    }

    ~BVHNode()
    {
        delete leftNode;
        delete rightNode;
    }

    bool isLeaf() const
    {
        return leftNode == nullptr && rightNode == nullptr;
    }

    bool isHit(const Ray &ray) const
    {
        return boundingBox.intersect(ray);
    }
};

#endif