#ifndef PLANE_H
#define PLANE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include <iostream>

using std::sqrt;

class plane : public objs {
    public: 
        plane(const point3& point, const vec3& normal) : a(point), n(normal) {}
        
        point3 point() const {
            return a;
        }

        vec3 normal() const {
            return n;
        }

        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 a;
        vec3 n;
};

double plane::ray_intersection(const ray& r, hit_record& rec) const {
    double t = dot((a - r.origin()), n) / dot(r.direction(), n);

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, n);
    return t;
}

#endif