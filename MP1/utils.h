#ifndef UTILS_H
#define UTILS_H

#include "ray.h"
#include "vec3.h"

// From RTioW
inline double random_double() {
    // Returns a random real in [0,1).
    srand(50);
    return rand() / (RAND_MAX + 1.0);
}

// From RTioW
inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

#endif