#ifndef __RAY_H__
#define __RAY_H__

#include <glm/glm.hpp>

struct HitResult
{
    float hitDistance;
    glm::vec3 hitWorldNormal;
    glm::vec3 hitWorldPosition;
};

struct Ray
{
    Ray(const glm::vec3 &origin, const glm::vec3 &direction)
        : origin(origin), direction(direction)
    {
    }

    glm::vec3 origin{};
    glm::vec3 direction{};
};
#endif // __RAY_H__