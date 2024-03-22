#include "geometry.hpp"

#include <glm/gtx/common.hpp>

// --------------------------------------------------------------------------
// Helper function

// --------------------------------------------------------------------------
// Geometry
BoundingBox Geometry::getBoundingBox(const std::vector<Model::Vertex> &vertices)
{
    return BoundingBox();
}
HitResult Geometry::getIntersection(const Ray &ray)
{
    return HitResult();
}

// --------------------------------------------------------------------------
// Triangle
BoundingBox Triangle::getBoundingBox(const std::vector<Model::Vertex> &vertices)
{
    BoundingBox boudingBox;
    boudingBox.vMin = glm::min(glm::min(vertices[v0].position, vertices[v1].position), vertices[v2].position);
    boudingBox.vMax = glm::max(glm::max(vertices[v0].position, vertices[v1].position), vertices[v2].position);

    return boudingBox;
}
HitResult Triangle::getIntersection(const Ray &ray)
{
}