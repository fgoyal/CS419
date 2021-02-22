#ifndef JITTER_H
#define JITTER_H

#include "color.h"
#include "ray.h"
#include "vec3.h"

// #include <ctime>
#include <cstdlib>
#include <random>
#include <map>
#include <iostream>

inline int random_int(int min, int max) {
    return rand() % max + min;
}

inline bool** get_multi_jitter_mask(int fine_grid) {
    int coarse_size = (int) sqrt(fine_grid);
    
    std::map<int, bool> row;
    std::map<int, bool> col;

    bool** sample;
    sample = new bool*[fine_grid];
    for (int i = 0; i < fine_grid; i++) {
        sample[i] = new bool[fine_grid];
        for (int j = 0; j < fine_grid; j++) {
            sample[i][j] = false;
        }
    }
    srand(time(NULL));
    for (int i = 0; i < coarse_size; i++) {
        for (int j = 0; j < coarse_size; j++) {
            std::cerr << i << " " << j << "\n";
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
            
            std::cerr << x << " " << y << "\n\n";
            sample[x][y] = true;
        }
    }
    return sample;
}

inline void display_jitter_mask(int fine_grid) {
    std::cout << "P3\n" << fine_grid << ' ' << fine_grid << "\n255\n";
    bool** sample = get_multi_jitter_mask(fine_grid);
        
    for (int j = 0; j < fine_grid; ++j) {
        std::cerr << "\rScanlines done: " << j << ' ' << std::flush;
        for (int i = 0; i < fine_grid; ++i) {
            if (sample[i][j]) {
                write_color(std::cout, color(0,0,0));
            } else {
                write_color(std::cout, color(1,1,1));
            }
        }
    }
}
#endif