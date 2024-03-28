#ifndef __HELPER_H__
#define __HELPER_H__

#include <glm/glm.hpp>

// std
#include <cstdlib>
#include <random>

namespace arc
{
    const double eps = 1e-8;
    inline double randomDouble()
    {
        // returns a random real in [0, 1]
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
    }

    inline double randomDouble(int min, int max)
    {
        return min + (max - min) * randomDouble();
    }

    inline bool isNearZero(const glm::vec3 &vec)
    {
        return (std::fabs(vec[0]) < eps && std::fabs(vec[1]) < eps && std::fabs(vec[2]) < eps);
    }

    static glm::vec3 randomInUniformSphere()
    {
        // rejection method
        while (true)
        {
            glm::vec3 randomDirection = {randomDouble(-1, 1), randomDouble(-1, 1), randomDouble(-1, 1)};
            if (glm::dot(randomDirection, randomDirection) <= 1.f)
            {
                return glm::normalize(randomDirection);
            }
        }
    }

    static glm::vec3 randomInHemisphere(const glm::vec3 &normal)
    {
        while (true)
        {
            glm::vec3 randomDirection = randomInUniformSphere();
            float result = glm::dot(normal, randomDirection);
            if (result > 0.f)
            {
                return randomDirection;
            }
            else
            {
                return -randomDirection;
            }
        }
    }
}

#endif