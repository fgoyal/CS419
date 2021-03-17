#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "objs.h"
#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "aabb.h"
#include <algorithm>
#include <vector>
#include <cstdlib>

using std::vector;

class bvh_node : public objs {
    public: 
        bvh_node() {};
        bvh_node(const vector<objs*>& objects, size_t start, size_t end);

        virtual color kDiffuse() const;
        virtual vec3 surface_normal(const point3 position) const;
        virtual bool ray_intersection(const ray& r, hit_record& rec) const;
        virtual aabb bounding_box() const;

    public:
        objs* left;
        objs* right;
        aabb bbox;
};

color bvh_node::kDiffuse() const {
    return color(-10,-10,-10);
}

vec3 bvh_node::surface_normal(const point3 position) const {
    return vec3(-10.0,-10.0,-10.0);
}

bool bvh_node::ray_intersection(const ray& r, hit_record& rec) const {
    if (!bbox.ray_intersection(r)) {
        return false;
    }

    double hit_left = left->ray_intersection(r, rec);
    double hit_right = right->ray_intersection(r, rec);

    return hit_left || hit_right;
}

aabb bvh_node::bounding_box() const {
    return bbox;
}

inline bool box_compare(const objs* a, const objs* b, int axis) {
    aabb box_a = a->bounding_box();
    aabb box_b = b->bounding_box();

    return box_a.centroid()[axis] < box_b.centroid()[axis];
}

bool box_x_compare(const objs* a, const objs* b) {
    return box_compare(a, b, 0);
}

bool box_y_compare(const objs* a, const objs* b) {
    return box_compare(a, b, 1);
}

bool box_z_compare(const objs* a, const objs* b) {
    return box_compare(a, b, 2);
}

bvh_node::bvh_node(const vector<objs*>& objects, size_t start, size_t end) {
    vector<objs*> objs = objects;
    // cerr << "node: " << start << " " << end << "\n";

    // Compute (xmin, ymin, zmin) and (xmax, ymax, zmax) for centroids
    double min[3];
    double max[3];
    bool first = true;
    for (int o = start; o < end; o++) {
        for (int i = 0; i < 3; i++) {
            double var = objects[o]->bounding_box().centroid()[i];
            if (first) {
                min[i] = max[i] = var;
            } else {
                if (var < min[i]) {
                    min[i] = var;
                }
                if (var > max[i]) {
                    max[i] = var;
                }
            }
        }
        first = false;
    }

    // pick axis based on largest spread
    int axis = 0;
    double range = max[0] - min[0];
    double yrange = max[1] - min[1];
    double zrange = max[2] - min[2];
    if (yrange > range) {
        axis = 1;
        range = yrange;
    }

    if (zrange > range) {
        axis = 2;
        range = zrange;
    }
    
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;
    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objs[start];
    } else if (object_span == 2) {
        if (comparator(objs[start], objs[start + 1])) {
            left = objs[start];
            right = objs[start + 1];
        } else {
            left = objs[start + 1];
            right = objs[start];
        }
    } else {
        // STOP SORTING AND JUST COMPARE EACH ONE TO MIDPOINT AND CREATE TWO LISTS, 
        // aka no need for start and stop in constructor
        std::sort(objs.begin() + start, objs.begin() + end, comparator);
        auto median_split = (max[axis] + min[axis]) / 2;
        int mid = start;
        for (int o = start; o < end; o++) {
            double curr = objs[o]->bounding_box().centroid()[axis];
            if (curr > median_split) {
                mid = fmax(mid, o);
                break;
            }
        }
        left = new bvh_node(objs, start, mid);
        right = new bvh_node(objs, mid, end);
    }

    aabb box_left = left->bounding_box();
    aabb box_right = right->bounding_box();
    
    bbox = surrounding_box(box_left, box_right);
}

#endif