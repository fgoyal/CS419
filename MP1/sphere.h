#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"
#include "ray.h"
#include <iostream>

using std::sqrt;

class sphere {
    public: 
        sphere(const point3& center, const double radius) : c(center), r(radius) {}
        point3 center() const {
            return c;
        }

        double radius() const {
            return r;
        }

        vec3 surface_normal(const point3 position) const {
            return unit_vector(position - c);
        }

    public:
        point3 c;
        double r;
};

inline double ray_sphere_intersection(const sphere& sphere, const ray& r) {
    vec3 oc = r.origin() - sphere.center();
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - sphere.radius() * sphere.radius();
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant)) / (a);
    }
}

#endif