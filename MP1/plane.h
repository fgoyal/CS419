#ifndef PLANE_H
#define PLANE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include <iostream>

using std::sqrt;

class plane : public objs {
    public: 
        plane(const point3& point, const vec3& normal, const color& kDiffuse) : a(point), n(normal), kD(kDiffuse) {}
        
        point3 point() const {
            return a;
        }

        // vec3 normal() const {
        //     return n;
        // }

        virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 a;
        vec3 n;
        color kD;
};
color plane::kDiffuse() const {
    return kD;
}

vec3 plane::surface_normal(const point3 position) const {
    return n;
}

double plane::ray_intersection(const ray& r, hit_record& rec) const {
    double t = dot((a - r.origin()), n) / dot(r.direction(), n);

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, n);
    return t;
}

#endif