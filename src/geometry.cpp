#include "geometry.hpp"
#include "helper.hpp"

// std
#include <algorithm>
#include <iostream>
// --------------------------------------------------------------------------
// Triangle

Triangle::Triangle(glm::dvec3 _v0, glm::dvec3 _v1, glm::dvec3 _v2)
    : v0(_v0), v1(_v1), v2(_v2)
{
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    glm::dvec3 crossProduct = glm::cross(edge1, edge2);
    faceNormal = glm::normalize(crossProduct);
    areaSize = 0.5f * glm::dot(crossProduct, crossProduct);
}

double Triangle::getArea()
{
    return areaSize;
}

void Triangle::setUVs(glm::dvec2 _uv0, glm::dvec2 _uv1, glm::dvec2 _uv2)
{
    uv0 = _uv0;
    uv1 = _uv1;
    uv2 = _uv2;
}

BoundingBox Triangle::getBoundingBox()
{
    BoundingBox boudingBox;

    boudingBox.merge(v0);
    boudingBox.merge(v1);
    boudingBox.merge(v2);

    return boudingBox;
}

HitResult Triangle::getIntersection(const Ray &ray)
{
    HitResult hitResult;
    if (glm::dot(ray.direction, glm::cross(edge1, edge1)) > 0)
        return hitResult;

    // 计算射线方向与边向量edge2的叉乘
    glm::dvec3 h = glm::cross(ray.direction, edge2);

    // 判断射线是否与三角形平面平行
    double angle = glm::dot(h, edge1);
    if (std::abs(angle) < EPSILON) // 使用std::abs来避免浮点数误差
        return hitResult;          // 如果平行,直接返回hitResult

    // 计算相交时的射线参数t
    glm::dvec3 side = ray.origin - v0;
    double u = glm::dot(side, h) / angle;

    // 判断相交点是否在三角形边界内
    if (u < 0.0f || u > 1.0f)
        return hitResult;

    glm::dvec3 side1 = glm::cross(side, edge1);
    double v = glm::dot(ray.direction, side1) / angle;
    if (v < 0.0f || u + v > 1.0f)
        return hitResult;

    // 计算相交时的射线参数t
    double time = glm::dot(edge2, side1) / angle;
    if (time > EPSILON)
    {
        hitResult.isHit = true;
        hitResult.hitWorldNormal = glm::normalize(glm::cross(edge1, edge2)); // 计算三角形法向量
        // 设定一定的偏移量
        hitResult.hitWorldPosition = ray.origin + ray.direction * time + hitResult.hitWorldNormal * OFFSET;
        hitResult.hitUV = (1. - u - v) * uv0 + u * uv1 + v * uv2;
        hitResult.mat = mat;
        hitResult.hitTime = time;
        hitResult.area = getArea();
        glm::dvec3 ray2Point = hitResult.hitWorldPosition - ray.origin;
        hitResult.distance = glm::dot(ray2Point, ray2Point);
    }
    return hitResult;
}

void Triangle::sample(HitResult &point, double &pdf)
{
    double u = std::sqrt(arc::randomDouble()), v = arc::randomDouble();
    point.hitWorldPosition = v0 * (1. - u) + v1 * (u * (1. - v)) + v2 * (u * v);
    point.hitWorldNormal = faceNormal;
    pdf = 1.f / areaSize;
}