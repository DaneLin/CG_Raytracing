#include "bvh.hpp"
#include "record.hpp"

static bool intersectRayBVH(const Ray &ray, const BVHNode *node, HitResult &hitResult, float &time)
{
    if (!node->isHit(ray))
        return false;
    bool isHit = false;
    if (node->leftNode == nullptr && node->rightNode == nullptr)
    {
        for (auto triangle : node->triangles)
        {
            HitResult tmpResult = triangle.getIntersection(ray);
            if (tmpResult.isHit && tmpResult.hitTime < time)
            {
                time = tmpResult.hitTime;
                hitResult = tmpResult;
                isHit = true;
            }
        }
    }
    else
    {
        bool isLeftHit = intersectRayBVH(ray, node->leftNode, hitResult, time);
        bool isRightHit = intersectRayBVH(ray, node->rightNode, hitResult, time);
        isHit = isLeftHit || isRightHit;
    }
    return isHit;
}