#ifndef __BVH_H__
#define __BVH_H__

#include "geometry.hpp"

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
            boundingBox.merge(triangle[0].position);
            boundingBox.merge(triangle[1].position);
            boundingBox.merge(triangle[2].position);
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