#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "utils.h"

#include <iostream>

void write_color(std::ostream &out, color pixel_color) {
    // Write the translated [0, 255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

color get_random_color() {
    return color(random_double(), random_double(), random_double());
}

color shade(color pixel_color, double factor) {
    return pixel_color * factor;
}

#endif