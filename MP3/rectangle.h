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
        rectangle(const double x0_t, const double x1_t, const double z0_t, const double z1_t, const double k_t, const color& kDiffuse, material* mat) : x0(x0_t), x1(x1_t), z0(z0_t), z1(z1_t), kD(kDiffuse), m(mat) {
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
        double x0, x1, z0, z1, k;
        color kD;
        aabb bbox;
        material* m;
};

vec3 rectangle::surface_normal(const point3 position) const {
    return vec3(0,1,0);
}

bool rectangle::ray_intersection(const ray& r, hit_record& rec, double tmin, double tmax) const {
    double t = (k - r.origin().y()) / r.direction().y();
    if (t < tmin || t > tmax) {
        return false;
    }

    double x = r.origin().x() + t * r.direction().x();
    double z = r.origin().z() + t * r.direction().z();

    if (x < x0 || x > x1 || z < z0 || z > z1) {
        return false;
    }

    rec.t = t;
    rec.p = r.at(t);
    rec.set_normal(r, surface_normal(rec.p));
    rec.kD = kD;
    rec.mat = m;
    return true;
}

aabb rectangle::create_aabb() const {
    return aabb(vec3(x0, k - 0.0001, z0), vec3(x1, k + 0.0001, z1));
}

inline ostream& operator<<(ostream &out, const rectangle& t) {
    return out << t.type() << ": " << t.kDiffuse();
}

#endif