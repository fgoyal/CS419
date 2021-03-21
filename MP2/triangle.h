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
        triangle(const vec3& a_t, const vec3& b_t, const vec3& c_t, const color& kDiffuse) : a(a_t), b(b_t), c(c_t), kD(kDiffuse) {
            bbox = create_aabb();
        }
        
        vec3 a_t() const {
            return a;
        }

        vec3 b_t() const {
            return b;
        }

        vec3 c_t() const {
            return c;
        }

        color kDiffuse() const {
            return kD;
        }

        aabb bounding_box() const {
            return bbox;
        }

        // virtual color kDiffuse() const;
        vec3 surface_normal(const point3 position) const;
        vec3 interpolated_normal(const point3 position) const;
        bool ray_intersection(const ray& r, hit_record& rec) const;
        aabb create_aabb() const;
        void set_vertex_normals(const vec3& a, const vec3& b, const vec3& c);
        vec3 barycentric_coordinates(const point3 position) const;

    public:
        point3 a;
        point3 b;
        point3 c;
        color kD;
        aabb bbox;
        vec3 normal_a;
        vec3 normal_b;
        vec3 normal_c;
};
inline double area(const point3 x, const point3 y, const point3 z) {
    vec3 e1 = y - x;
    vec3 e2 = z - x;
    return 0.5 * cross(e1, e2).length();
}

vec3 triangle::surface_normal(const point3 position) const {
    vec3 e1 = b - a;
    vec3 e2 = c - a;
    return unit_vector(cross(e1, e2));
}

vec3 triangle::interpolated_normal(const point3 position) const {
    vec3 bc = barycentric_coordinates(position);
    // cerr << "bary: " << bc << "\n";
    float x = normal_a[0] * bc[0] + normal_b[0] * bc[1] + normal_c[0] * bc[2];
    float y = normal_a[1] * bc[0] + normal_b[1] * bc[1] + normal_c[1] * bc[2];
    float z = normal_a[2] * bc[0] + normal_b[2] * bc[1] + normal_c[2] * bc[2];
    return vec3(x, y, z);
    // return normal_c;
    // return normal_a * bc[0] + normal_b * bc[1] + normal_c * bc[2];
}

bool triangle::ray_intersection(const ray& r, hit_record& rec) const {
    // vec3 N = surface_normal(point3(0.0,0.0,0.0));

    vec3 e1 = b - a;
    vec3 e2 = c - a;
    vec3 q = cross(r.direction(), e2);

    double p = dot(e1, q);
    if (abs(p) < 0.000001) {
        return false;
    }

    double f = 1/p;
    vec3 s = r.origin() - a;
    double u = f * dot(s, q);

    if (u < 0.0) {
        return false;
    }

    vec3 x = cross(s, e1);
    double v = f * dot(r.direction(), x);
    if (v < 0.0 || (u + v) > 1.0) {
        return false;
    }
    double t = f * dot(e2, x);
    rec.t = t;
    rec.p = r.at(t);
    // rec.set_normal(r, interpolated_normal(rec.p));
    rec.set_normal(r, surface_normal(rec.p));
    rec.kD = kD;
    return true;
}

aabb triangle::create_aabb() const {
    double minx = fmin(fmin(a[0], b[0]), c[0]);
    double maxx = fmax(fmax(a[0], b[0]), c[0]);
    double miny = fmin(fmin(a[1], b[1]), c[1]);
    double maxy = fmax(fmax(a[1], b[1]), c[1]);
    double minz = fmin(fmin(a[2], b[2]), c[2]);
    double maxz = fmax(fmax(a[2], b[2]), c[2]);

    return aabb(vec3(minx, miny, minz), vec3(maxx, maxy, maxz));
}

void triangle::set_vertex_normals(const vec3& x, const vec3& y, const vec3& z) {
    normal_a = x;
    normal_b = y;
    normal_c = z;
}

vec3 triangle::barycentric_coordinates(const point3 position) const {
    double T = area(a, b, c);
    // cerr << T << "\n";
    double b1 = area(position, b, c) / T;
    double b2 = area(a, position, c) / T;
    double b3 = area(a, b, position) / T;
    // cerr << b1 << "\n";
    // cerr << b2 << "\n";
    // cerr << b3 << "\n";
    return vec3(b1, b2, b3);
}

#endif