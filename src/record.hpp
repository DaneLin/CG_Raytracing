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
    float hitTime = std::numeric_limits<float>::max();
    int faceMaterialID = -1;
    glm::vec3 hitWorldNormal{};
    glm::vec3 hitWorldPosition{};
    glm::vec2 hitUV{};
};
#endif