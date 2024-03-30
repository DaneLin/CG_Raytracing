// definitions of hitting records

#ifndef __RECORD_H__
#define __RECORD_H__

#include <glm/glm.hpp>

class Ray;
class Material;

struct ScatteredResult
{
    glm::dvec3 attenuation;
    bool skipPDF;
    Ray rayOut;
};

struct HitResult
{
    bool isHit = false;
    double distance = std::numeric_limits<double>::max();
    std::shared_ptr<Material> mat;
    double area = std::numeric_limits<double>::max();
    glm::dvec3 hitWorldNormal{};
    glm::dvec3 hitWorldPosition{};
    double hitTime = std::numeric_limits<double>::max();
    glm::vec2 hitUV{};
};
#endif