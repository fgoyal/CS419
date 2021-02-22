#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"

class triangle : public objs {
    public: 
        triangle(const vec3& a_t, const vec3& b_t, const vec3& c_t) : a(a_t), b(b_t), c(c_t) {}
        
        vec3 a_t() const {
            return a;
        }

        vec3 b_t() const {
            return b;
        }

        vec3 c_t() const {
            return c;
        }

        vec3 normal() const {
            vec3 e1 = c - b;
            vec3 e2 = a - c;
            return unit_vector(cross(e1, e2));
        }

        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 a;
        point3 b;
        point3 c;
        vec3 n;
};

double triangle::ray_intersection(const ray& r, hit_record& rec) const {
    // double t = dot((a - r.origin()), n) / dot(r.direction(), n);
    vec3 N = normal();
    double nraydir = dot(N, r.direction());
    if (abs(nraydir) < 0.0001) {
        return -1;
    }

    double d = dot(N, a);
    double t = (dot(N, r.origin()) + d) / nraydir;

    if (t < 0) {
        return -1;
    }

    vec3 p = r.origin() + t * r.direction();
    vec3 x;

    vec3 edge0 = b - a;
    vec3 vp0 = p - a;
    x = cross(edge0, vp0);
    if (dot(N, x) < 0) {
        return -1;
    }

    vec3 edge1 = c - b;
    vec3 vp1 = p - b;
    x = cross(edge1, vp1);
    if (dot(N, x) < 0) {
        return -1;
    }

    vec3 edge2 = a - c;
    vec3 vp2 = p - c;
    x = cross(edge2, vp2);
    if (dot(N, x) < 0) {
        return -1;
    }

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, n);
    return t;
}

#endif