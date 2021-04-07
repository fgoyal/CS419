#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"
#include "material.h"

class rectangle : public objs {
    public: 
        /** 
         * Constructor for a Triangle
         * @param a_t, b_t, c_t: the three edge points of the triangle
         * @param kDiffuse the kDiffuse element for the Phong shading model
         */
        rectangle(const vec3& a_, const vec3& b_, const vec3& c_, const vec3& d_, const color& kDiffuse, material* mat) : a(a_), b(b_), c(c_), d(d_), kD(kDiffuse), m(mat) {
            bbox = create_aabb();
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

        std::string type() const {
            return "rectangle";
        }

        // virtual color kDiffuse() const;
        vec3 surface_normal(const point3 position) const;
        bool ray_intersection(const ray& r, hit_record& rec, double tmin, double tmax) const;
        aabb create_aabb() const;

    public:
        vec3 a,b,c,d;
        color kD;
        aabb bbox;
        material* m;
};

vec3 rectangle::surface_normal(const point3 position) const {
    return vec3(0,1,0);
}

bool rectangle::ray_intersection(const ray& r, hit_record& rec, double tmin, double tmax) const {
    // double t = (k - r.origin().y()) / r.direction().y();
    // if (t < tmin || t > tmax) {
    //     return false;
    // }

    // double x = r.origin().x() + t * r.direction().x();
    // double z = r.origin().z() + t * r.direction().z();

    // if (x < x0 || x > x1 || z < z0 || z > z1) {
    //     return false;
    // }

    // rec.t = t;
    // rec.p = r.at(t);
    // rec.set_normal(r, surface_normal(rec.p));
    // rec.kD = kD;
    // rec.mat = m;
    return true;
}

aabb rectangle::create_aabb() const {
    // return aabb(vec3(x0, k - 0.0001, z0), vec3(x1, k + 0.0001, z1));
    return aabb(a, b);
}

inline ostream& operator<<(ostream &out, const rectangle& t) {
    return out << t.type() << ": " << t.kDiffuse();
}

#endif