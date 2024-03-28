#ifndef __SCENE_H__
#define __SCENE_H__

#include "geometry.hpp"
#include "material.hpp"
#include "bvh.hpp"

// std
#include <vector>

class Scene
{
public:
    BVHNode *createBVH(std::vector<Triangle> &triangels);
    void buildBVH();

    Scene(std::vector<Triangle> &geo, std::vector<std::shared_ptr<Material>> &materials)
        : geometries(geo), materials(materials)
    {
        this->buildBVH();
    }
    void setGeometries(std::vector<Triangle> &geo) { geometries = geo; }
    const std::vector<Triangle> &getGeometries() const { return geometries; }
    const std::vector<std::shared_ptr<Material>> &getMaterials() const { return materials; }
    const BVHNode *getBVHTreeRoot() const { return node; }

private:
private:
    std::vector<Triangle> &geometries;
    std::vector<std::shared_ptr<Material>> materials;
    BVHNode *node = nullptr;
};

#endif