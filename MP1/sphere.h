#ifndef SPHERE_H
#define SPHERE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"

using std::sqrt;

class sphere : public objs {
    public: 
        sphere(const point3& center, const double radius, const color& kDiffuse) : c(center), rad(radius), kD(kDiffuse) {}
        point3 center() const {
            return c;
        }

        double radius() const {
            return rad;
        }

        virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual double ray_intersection(const ray& r, hit_record& rec) const;

    public:
        point3 c;
        double rad;
        vec3 kD;
};

color sphere::kDiffuse() const {
    return kD;
}

vec3 sphere::surface_normal(const point3 position) const {
    return unit_vector(position - c);
}

double sphere::ray_intersection(const ray& r, hit_record& rec) const {
    vec3 oc = r.origin() - c;
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - rad * rad;
    double discriminant = half_b * half_b - a * c;
    double root;
    if (discriminant < 0) {
        return -1.0; // no intersection
    } 
    
    root = (-half_b - sqrt(discriminant)) / (a);

    rec.t = root;
    rec.p = r.at(root);
    rec.set_normal(r, surface_normal(rec.p));

    return root;
}

#endif