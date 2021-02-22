#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"

class triangle : public objs {
    public: 
        /** 
         * Constructor for a Triangle
         * @param a_t, b_t, c_t: the three edge points of the triangle
         * @param kDiffuse the kDiffuse element for the Phong shading model
         */
        triangle(const vec3& a_t, const vec3& b_t, const vec3& c_t, const color& kDiffuse) : a(a_t), b(b_t), c(c_t), kD(kDiffuse) {}
        
        vec3 a_t() const {
            return a;
        }

        vec3 b_t() const {
            return b;
        }

        vec3 c_t() const {
            return c;
        }

        virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 a;
        point3 b;
        point3 c;
        color kD;
};

color triangle::kDiffuse() const {
    return kD;
}

vec3 triangle::surface_normal(const point3 position) const {
    vec3 e1 = c - b;
    vec3 e2 = a - c;
    return unit_vector(cross(e1, e2));
}

double triangle::ray_intersection(const ray& r, hit_record& rec) const {
    vec3 N = surface_normal(point3(0.0,0.0,0.0));
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
    rec.set_normal(r, N);
    rec.kD = kD;
    return t;
}

#endif