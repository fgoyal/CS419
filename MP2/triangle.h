#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"

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
        virtual bool bounding_box(aabb& bbox) const;

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
    vec3 e1 = b - a;
    vec3 e2 = c - a;
    return unit_vector(cross(e1, e2));
}

double triangle::ray_intersection(const ray& r, hit_record& rec) const {
    vec3 N = surface_normal(point3(0.0,0.0,0.0));

    vec3 e1 = b - a;
    vec3 e2 = c - a;
    vec3 q = cross(r.direction(), e2);

    double p = dot(e1, q);
    if (abs(p) < 0.000001) {
        return -1;
    }

    double f = 1/p;
    vec3 s = r.origin() - a;
    double u = f * dot(s, q);

    if (u < 0.0) {
        return -1;
    }

    vec3 x = cross(s, e1);
    double v = f * dot(r.direction(), x);
    if (v < 0.0 || (u + v) > 1.0) {
        return -1;
    }
    double t = f * dot(e2, x);
    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, N);
    rec.kD = kD;
    return t;
}

bool triangle::bounding_box(aabb& bbox) const {
    double minx = fmin(fmin(a[0], b[0]), c[0]);
    double maxx = fmax(fmax(a[0], b[0]), c[0]);
    double miny = fmin(fmin(a[1], b[1]), c[1]);
    double maxy = fmax(fmax(a[1], b[1]), c[1]);
    double minz = fmin(fmin(a[2], b[2]), c[2]);
    double maxz = fmax(fmax(a[2], b[2]), c[2]);

    bbox = aabb(vec3(minx, miny, minz), vec3(maxx, maxy, maxz));

    return true;
}

#endif