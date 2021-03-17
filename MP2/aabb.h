#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"

class aabb {
    public:
        aabb() {};
        aabb(const point3& p0, const point3& p1) : minimum(p0), maximum(p1) {
            center = calculate_centroid();
        }
        
        point3 min() const {
            return minimum;
        }

        point3 max() const {
            return maximum;
        }

        point3 centroid() const {
            return center;
        }
        
        virtual bool ray_intersection(const ray& r) const;
        point3 calculate_centroid() const;

    public:
        point3 minimum;
        point3 maximum;
        point3 center;
};

bool aabb::ray_intersection(const ray& r) const {
    for (int i = 0; i < 3; i++) {
        double a = (minimum[i] - r.origin()[i]) / r.direction()[i];
        double b = (maximum[i] - r.origin()[i]) / r.direction()[i];
        if (fmax(a,b) <= fmin(a,b)) {
            return false;
        }
    }
    return true;
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

point3 aabb::calculate_centroid() const {
    double x = (maximum[0] + minimum[0]) / 2;
    double y = (maximum[1] + minimum[1]) / 2;
    double z = (maximum[2] + minimum[2]) / 2;
    return point3(x, y, z);
}

#endif