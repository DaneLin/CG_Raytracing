#include "bvh.hpp"
#include "record.hpp"

// std
#include <algorithm>

// BVHNode *BVHAccessor::buildBVH(std::vector<Triangle> tris)
// {
//     BVHNode *node = new BVHNode();

//     // compute bounding boxs of current triangles;
//     BoundingBox curBoundingBox;
//     for (auto &tri : tris)
//     {
//         unionBox(curBoundingBox, tri.getBoundingBox());
//     }

//     if (tris.size() == 1)
//     {
//         node->boundingBox = tris[0].getBoundingBox();
//         node->object = &tris[0];
//         node->leftNode = nullptr;
//         node->rightNode = nullptr;
//         node->area = tris[0].getArea();
//         return node;
//     }
//     else if (tris.size() == 2)
//     {
//         node->leftNode = buildBVH(std::vector{tris[0]});
//         node->rightNode = buildBVH(std::vector{tris[1]});

//         node->boundingBox = unionBox(node->leftNode->boundingBox, node->rightNode->boundingBox);
//         node->area = node->leftNode->area + node->rightNode->area;
//         return node;
//     }
//     else
//     {
//         BoundingBox centroidBoundingBox;
//         for (auto &tri : tris)
//         {
//             unionBox(centroidBoundingBox, tri.getBoundingBox().centroid());
//         }

//         int axis = centroidBoundingBox.maxAxis();
//         if (axis == 0)
//         {
//             std::sort(tris.begin(), tris.end(), [](auto tri1, auto tri2)
//                       { return tri1.getBoundingBox().centroid().x < tri2.getBoundingBox().centroid().x; });
//         }
//         else if (axis == 1)
//         {
//             std::sort(tris.begin(), tris.end(), [](auto tri1, auto tri2)
//                       { return tri1.getBoundingBox().centroid().y < tri2.getBoundingBox().centroid().y; });
//         }
//         else
//         {
//             std::sort(tris.begin(), tris.end(), [](auto tri1, auto tri2)
//                       { return tri1.getBoundingBox().centroid().z < tri2.getBoundingBox().centroid().z; });
//         }

//         auto middle = tris.begin() + tris.size() / 2;
//         auto leftTris = std::vector<Triangle>(tris.begin(), middle);
//         auto rightTris = std::vector<Triangle>(middle, tris.end());

//         node->leftNode = buildBVH(leftTris);
//         node->rightNode = buildBVH(rightTris);

//         node->boundingBox = unionBox(node->leftNode->boundingBox, node->rightNode->boundingBox);
//         node->area = node->leftNode->area + node->rightNode->area;
//     }

//     return node;
// }
// void BVHAccessor::getSample(BVHNode *node, float p, HitResult &hitResult, float &pdf)
// {
//     if (node->leftNode == nullptr || node->rightNode == nullptr)
//     {
//         node->object->sample(hitResult, pdf);
//         pdf *= node->area;
//         return;
//     }
//     if (p < node->leftNode->area)
//         getSample(node->leftNode, p, hitResult, pdf);
//     else
//         getSample(node->rightNode, p - node->leftNode->area, hitResult, pdf);
// }
// void BVHAccessor::sample(HitResult &hitResult, float &pdf)
// {
//     float p = std::sqrt(arc::randomDouble()) * root->area;
//     getSample(root, p, hitResult, pdf);
//     pdf /= root->area;
// }

// HitResult BVHAccessor::getIntersection(BVHNode *node, const Ray &ray) const
// {
//     HitResult hitResult;

//     if (node->boundingBox.intersect(ray) == false)
//         return hitResult;

//     if (node->leftNode == nullptr && node->rightNode == nullptr)
//     {
//         hitResult = node->object->getIntersection(ray);
//         return hitResult;
//     }

//     auto result1 = getIntersection(node->leftNode, ray);
//     auto result2 = getIntersection(node->rightNode, ray);

//     if (result1.distance > result2.distance)
//         return result2;
//     return result1;
// }

static HitResult intersectRayBVH(const Ray &ray, const BVHNode *node)
{
    HitResult res;
    if (!node->isHit(ray))
        return res;
    if (node->leftNode == nullptr && node->rightNode == nullptr)
    {
        for (auto triangle : node->triangles)
        {
            HitResult tmpResult = triangle.getIntersection(ray);
            return tmpResult;
        }
    }

    HitResult isLeftHit = intersectRayBVH(ray, node->leftNode);
    HitResult isRightHit = intersectRayBVH(ray, node->rightNode);

    if (isLeftHit.hitTime > isRightHit.hitTime)
        return isRightHit;
    return isLeftHit;
}