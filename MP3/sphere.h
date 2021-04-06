#ifndef SPHERE_H
#define SPHERE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"
#include "material.h"

using std::sqrt;

class sphere : public objs {
    public: 
        /** 
         * Constructor for a Sphere
         * @param center the center point of the sphere
         * @param radius the radius for the sphere
         * @param kDiffuse the kDiffuse element for the Phong shading model
         */
        sphere(const point3& center, const double radius, const color& kDiffuse, material* mat) : c(center), rad(radius), kD(kDiffuse), m(mat) {
            bbox = create_aabb();
        }
        point3 center() const {
            return c;
        }

        double radius() const {
            return rad;
        }

        color kDiffuse() const {
            return kD;
        }

        material* mat() const {
            return m;
        }

        aabb bounding_box() const {
            return bbox;
        }

        // virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual bool ray_intersection(const ray& r, hit_record& rec) const;
        aabb create_aabb() const;

    public:
        point3 c;
        double rad;
        color kD;
        aabb bbox;
        material* m;
};

// color sphere::kDiffuse() const {
//     return kD;
// }

vec3 sphere::surface_normal(const point3 position) const {
    return unit_vector(position - c);
}

bool sphere::ray_intersection(const ray& r, hit_record& rec) const {
    vec3 oc = r.origin() - c;
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - rad * rad;
    double discriminant = half_b * half_b - a * c;
    double root;
    if (discriminant < 0) {
        return false; // no intersection
    } 
    
    root = (-half_b - sqrt(discriminant)) / (a);

    rec.t = root;
    rec.p = r.at(root);
    rec.set_normal(r, surface_normal(rec.p));
    rec.kD = kD;
    rec.mat = m;
    // return true;
    return (root >= 0.0);
}

aabb sphere::create_aabb() const {
    return aabb(
        c - vec3(rad, rad, rad),
        c + vec3(rad, rad, rad)
    );
}

#endif