#ifndef PLANE_H
#define PLANE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"

using std::sqrt;

class plane : public objs {
    public: 
        /** 
         * Constructor for a Plane
         * @param point any point that appears on the plane
         * @param normal the surface normal for the plane
         * @param kDiffuse the kDiffuse element for the Phong shading model
         */
        plane(const point3& point, const vec3& normal, const color& kDiffuse) : a(point), n(normal), kD(kDiffuse) {}
        
        point3 point() const {
            return a;
        }

        virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual bool ray_intersection(const ray& r, hit_record& rec) const;
        virtual bool bounding_box(aabb& bbox) const;

    public:
        point3 a;
        vec3 n;
        color kD;
};

color plane::kDiffuse() const {
    return kD;
}

vec3 plane::surface_normal(const point3 position) const {
    return unit_vector(n);
}

bool plane::ray_intersection(const ray& r, hit_record& rec) const {
    double t = dot((a - r.origin()), unit_vector(n)) / dot(r.direction(), unit_vector(n));

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, unit_vector(n));
    rec.kD = kD;
    return (t >= 0.0);
}

bool plane::bounding_box(aabb& bbox) const {
    return true;
}

#endif