#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"

class aabb {
    public:
        aabb() {};
        aabb(const point3& p0, const point3& p1) : minimum(p0), maximum(p1) {}
        
        point3 min() const {
            return minimum;
        }

        point3 max() const {
            return maximum;
        }
        
        virtual double ray_intersection(const ray& r, double min, double max) const;

    public:
        point3 minimum;
        point3 maximum;
};

double aabb::ray_intersection(const ray& r, double min, double max) const {
    for (int i = 0; i < 3; i++) {
        double a = (minimum[i] - r.origin()[i]) / r.direction()[i];
        double b = (maximum[i] - r.origin()[i]) / r.direction()[i];
        min = fmin(fmin(a,b), min);
        max = fmax(fmax(a,b), max);
        if (max <= min) {
            return -1;
        }
    }
    return 1;
}

inline aabb surrounding_box(const aabb& a, const aabb& b) {
    point3 p0(  fmin(a.min().x(), b.min().x()),
                fmin(a.min().y(), b.min().y()),
                fmin(a.min().z(), b.min().z()));
    point3 p1(  fmax(a.max().x(), b.max().x()),
                fmax(a.max().y(), b.max().y()),
                fmax(a.max().z(), b.max().z()));
    return aabb(p0, p1);
}

#endif