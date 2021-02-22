#ifndef OBJS_H
#define OBJS_H

#include "ray.h"
#include "vec3.h"

struct hit_record {
    point3 p;
    vec3 normal;
    double t;

    inline void set_normal(const ray& r, const vec3& n) {
        bool front = dot(r.direction(), n) < 0;
        normal = front ? n : -n;
    }
};

class objs {
    public:
        virtual double ray_intersection(const ray& r, hit_record& rec) const = 0;

};

#endif