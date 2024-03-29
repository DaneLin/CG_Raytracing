// definitions of hitting records

#ifndef __RECORD_H__
#define __RECORD_H__

#include <glm/glm.hpp>

class Ray;
class Material;

struct ScatteredResult
{
    glm::vec3 attenuation;
    bool skipPDF;
    Ray rayOut;
};

struct HitResult
{
    bool isHit = false;
    double distance = std::numeric_limits<double>::max();
    std::shared_ptr<Material> mat;
    float area = std::numeric_limits<float>::max();
    glm::vec3 hitWorldNormal{};
    glm::vec3 hitWorldPosition{};
    double hitTime = std::numeric_limits<double>::max();
    glm::vec2 hitUV{};
};
#endif