#ifndef __HELPER_H__
#define __HELPER_H__

#include <cstdlib>
#include <random>

inline float randomFloat()
{
    // returns a random real in [0, 1]
    return std::rand() / (RAND_MAX + 1.f);
}

#endif