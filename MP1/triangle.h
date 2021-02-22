#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"

class triangle : public objs {
    public: 
        triangle(const point3& a_t, const point3& b_t, const point3& c_t, const vec3& normal) : a(a_t), b(b_t), c(c_t), n(normal) {}
        
        point3 a_t() const {
            return a;
        }

        point3 b_t() const {
            return b;
        }

        point3 c_t() const {
            return c;
        }

        vec3 normal() const {
            return n;
        }

        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 a;
        point3 b;
        point3 c;
        vec3 n;
};

double triangle::ray_intersection(const ray& r, hit_record& rec) const {
    double t = dot((a - r.origin()), n) / dot(r.direction(), n);

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, n);
    return t;
}

#endif