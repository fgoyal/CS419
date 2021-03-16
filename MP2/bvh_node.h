#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"

class bvh_node : public objs {
    public: 
        bvh_node() {};
        
        virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual bool ray_intersection(const ray& r, hit_record& rec) const;
        virtual bool bounding_box(aabb& bbox) const;

    public:
        objs* left;
        objs* right;
        aabb box;
};

color bvh_node::kDiffuse() const {
    return color(-10,-10,-10);
}

vec3 bvh_node::surface_normal(const point3 position) const {
    return vec3(-10.0,-10.0,-10.0);
}

bool bvh_node::ray_intersection(const ray& r, hit_record& rec) const {
    if (!box.ray_intersection(r)) {
        return -1;
    }

    double hit_left = left->ray_intersection(r, rec);
    double hit_right = right->ray_intersection(r, rec);

    return hit_left || hit_right;
}

bool bvh_node::bounding_box(aabb& bbox) const {
    bbox = box;
    return true;
}

#endif