#ifndef __SCENE_H__
#define __SCENE_H__

#include "geometry.hpp"

#include "bvh.hpp"

// std
#include <vector>

class Scene
{
public:
    BVHNode *createBVH(std::vector<Triangle> &triangels);
    void buildBVH();

    Scene(std::vector<Triangle> &geo)
        : geometries(geo)
    {
        this->buildBVH();
    }
    void setGeometries(std::vector<Triangle> &geo) { geometries = geo; }
    const std::vector<Triangle> &getGeometries() const { return geometries; }
    const BVHNode *getBVHTreeRoot() const { return node; }

private:
private:
    std::vector<Triangle> &geometries;
    BVHNode *node = nullptr;
};

#endif