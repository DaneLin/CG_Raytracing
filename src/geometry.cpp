#include "geometry.hpp"

// std
#include <algorithm>
#include <iostream>

// --------------------------------------------------------------------------
// Helper function

// --------------------------------------------------------------------------
// Geometry
BoundingBox Geometry::getBoundingBox()
{
    return BoundingBox();
}
HitResult Geometry::getIntersection(const Ray &ray)
{
    return HitResult();
}

// --------------------------------------------------------------------------
// Triangle
BoundingBox Triangle::getBoundingBox()
{
    BoundingBox boudingBox;

    boudingBox.merge(v0.position);
    boudingBox.merge(v1.position);
    boudingBox.merge(v2.position);

    return boudingBox;
}

HitResult Triangle::getIntersection(const Ray &ray)
{
    HitResult hitResult;

    // 计算三角形的两条边向量
    glm::vec3 edge1 = v1.position - v0.position;
    glm::vec3 edge2 = v2.position - v0.position;

    // 计算射线方向与边向量edge2的叉乘
    glm::vec3 h = glm::cross(ray.direction, edge2);

    // 判断射线是否与三角形平面平行
    float angle = glm::dot(h, edge1);
    if (std::abs(angle) < EPSILON) // 使用std::abs来避免浮点数误差
        return hitResult;          // 如果平行,直接返回hitResult

    // 计算相交时的射线参数t
    glm::vec3 side = ray.origin - v0.position;
    float u = glm::dot(side, h) / angle;

    // 判断相交点是否在三角形边界内
    if (u < 0.0f || u > 1.0f)
        return hitResult;

    glm::vec3 side1 = glm::cross(side, edge1);
    float v = glm::dot(ray.direction, side1) / angle;
    if (v < 0.0f || u + v > 1.0f)
        return hitResult;

    // 计算相交时的射线参数t
    float time = glm::dot(edge2, side1) / angle;
    if (time > EPSILON)
    {
        hitResult.isHit = true;
        hitResult.hitWorldNormal = glm::normalize(glm::cross(edge1, edge2)); // 计算三角形法向量
        // 设定一定的偏移量
        hitResult.hitWorldPosition = ray.origin + ray.direction * time + hitResult.hitWorldNormal * OFFSET;
        hitResult.hitUV = (1 - u - v) * v0.uv + u * v1.uv + v * v2.uv;
        hitResult.faceMaterialID = v0.faceMaterialID;
        hitResult.hitTime = time;
    }
    return hitResult;
}