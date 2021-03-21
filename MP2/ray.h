#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include <iostream>
using std::ostream;

class ray {
    public:
        ray() {}
        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

        point3 origin() const {
            return orig;
        }

        vec3 direction() const {
            return dir;
        }

        point3 at (double t) const {
            return orig + t * dir;
        }

        double euclidean_distance(const point3& p) {
            double a = pow(orig.x() - p.x(), 2);
            double b = pow(orig.y() - p.y(), 2);
            double c = pow(orig.z() - p.z(), 2);
            return sqrt(a + b + c);
        }

        // std::ostream& operator<<(std::ostream &out, ray const& r) {
        //     out << "ray: o - " << origin << " d - " << direction;
        //     return out;
        // }
    
    public:
        point3 orig;
        vec3 dir;
};

inline ostream& operator<<(ostream &out, const ray &r) {
    return out << "ray: o - " << r.orig << " d - " << r.dir;
}

#endif