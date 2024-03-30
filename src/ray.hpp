#ifndef __RAY_H__
#define __RAY_H__

#include <glm/glm.hpp>

class Ray
{
public:
    Ray() = default;
    Ray(const glm::dvec3 &origin, const glm::dvec3 &direction)
        : origin(origin), direction(direction)
    {
    }

    glm::dvec3 origin{};
    glm::dvec3 direction{};
};
#endif // __RAY_H__