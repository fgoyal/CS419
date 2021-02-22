#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"
#include "ray.h"
#include <iostream>

using std::sqrt;

class sphere {
    public: 
        sphere(const point3& center, const double radius, const vec3 kDiffuse) : c(center), rad(radius), kD(kDiffuse) {}
        point3 center() const {
            return c;
        }

        double radius() const {
            return rad;
        }

        vec3 kDiffuse() const {
            return kD;
        }

        vec3 surface_normal(const point3 position) const {
            return unit_vector(position - c);
        }

        double ray_intersection(const ray& r) const;

    public:
        point3 c;
        double rad;
        vec3 kD;
};

double sphere::ray_intersection(const ray& r) const {
    vec3 oc = r.origin() - c;
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - rad * rad;
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant)) / (a);
    }
}

#endif