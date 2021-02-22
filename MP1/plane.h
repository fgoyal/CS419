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

        vec3 normal() const {
            return n;
        }

        color kDiffuse() const {
            return kD;
        }

        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 a;
        vec3 n;
        color kD;
};

double plane::ray_intersection(const ray& r, hit_record& rec) const {
    double t = dot((a - r.origin()), n) / dot(r.direction(), n);

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, n);
    return t;
}

#endif