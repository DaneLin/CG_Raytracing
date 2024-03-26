#include "scene.hpp"

#include <algorithm>

void Scene::buildBVH()
{
    node = createBVH(geometries);
}

BVHNode *Scene::createBVH(std::vector<Triangle> &triangels)
{
    if (triangels.empty())
        return nullptr;

    BVHNode *bvhNode = new BVHNode(triangels);

    if (triangels.size() == 1)
        return bvhNode;

    // 使用最长的坐标轴进行划分
    float xRange = bvhNode->boundingBox.vMax.x - bvhNode->boundingBox.vMin.x;
    float yRange = bvhNode->boundingBox.vMax.y - bvhNode->boundingBox.vMin.y;
    float zRange = bvhNode->boundingBox.vMax.z - bvhNode->boundingBox.vMin.z;

    int axis = 0; // 默认用x
    if (xRange > yRange && xRange > zRange)
        axis = 0;
    else if (yRange > zRange)
        axis = 1;
    else
        axis = 2;

    // 排序
    std::vector<Triangle> sortedTriangle = triangels;
    if (axis == 0)
    {
        std::sort(sortedTriangle.begin(), sortedTriangle.end(), [](Triangle &a, Triangle &b)
                  { return a.getBoundingBox().vMin.x < b.getBoundingBox().vMin.x; });
    }
    else if (axis == 1)
    {
        std::sort(sortedTriangle.begin(), sortedTriangle.end(), [](Triangle &a, Triangle &b)
                  { return a.getBoundingBox().vMin.y < b.getBoundingBox().vMin.y; });
    }
    else
    {
        std::sort(sortedTriangle.begin(), sortedTriangle.end(), [](Triangle &a, Triangle &b)
                  { return a.getBoundingBox().vMin.z < b.getBoundingBox().vMin.z; });
    }

    std::vector<Triangle> leftTriangles(sortedTriangle.begin(), sortedTriangle.begin() + sortedTriangle.size() / 2);
    std::vector<Triangle> rightTriangles(sortedTriangle.begin() + sortedTriangle.size() / 2, sortedTriangle.end());

    bvhNode->leftNode = createBVH(leftTriangles);
    bvhNode->rightNode = createBVH(rightTriangles);

    return bvhNode;
}