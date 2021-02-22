#ifndef UTILS_H
#define UTILS_H

#include "ray.h"
#include "vec3.h"
#include <ctime>
#include <cstdlib>
#include <random>
#include <map>

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

inline int random_int(int min, int max) {
    // Returns a random int in [min,max).
    // srand(time(NULL));
    return rand() % max + min;
}

inline bool** get_multi_jitter_mask(int fine_size) {
    int coarse_size = (int) sqrt(fine_size);
    
    std::map<int, bool> row;
    std::map<int, bool> col;

    bool** sample;
    sample = new bool*[fine_size];
    for (int i = 0; i < fine_size; i++) {
        sample[i] = new bool[fine_size];
        for (int j = 0; j < fine_size; j++) {
            sample[i][j] = false;
        }
    }
    srand(time(NULL));
    for (int i = 0; i < coarse_size; i++) {
        for (int j = 0; j < coarse_size; j++) {
            // std::cerr << i << " " << j << "\n";
            int x;
            int y;
            do {
                x = random_int(0, coarse_size) + (i * coarse_size);
            } while (row.count(x) > 0);
            row.insert(std::pair<int,bool>(x,true));
            
            do {
                y = random_int(0, coarse_size) + (j * coarse_size);
            } while (col.count(y) > 0);
            col.insert(std::pair<int,bool>(y,true));
            
            // std::cerr << x << " " << y << "\n\n";
            sample[x][y] = true;
        }
    }
    return sample;
}

#endif