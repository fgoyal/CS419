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
        return false;
    }

    double hit_left = left->ray_intersection(r, rec);
    double hit_right = right->ray_intersection(r, rec);

    return hit_left || hit_right;
}

aabb bvh_node::bounding_box() const {
    return box;
}

inline bool box_compare(const objs* a, const objs* b, int axis) {
    aabb box_a = a->bounding_box();
    aabb box_b = b->bounding_box();

    // if (!a->bounding_box(box_a) || !b->bounding_box(box_b)) {
    //     std::cerr << "No bounding box in bvh constructor.\n";
    // }
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
    // std::cerr << "new node: " << start << " " << end << "\n";

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
            // cerr << var << " ";
        }
        // cerr << "\n";
        first = false;
    }

    // std::cerr << "\n\n\n" << min[0] << " " << min[1] << " " << min[2] << "\n";
    // std::cerr << max[0] << " " << max[1] << " " << max[2] << "\n";

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
    
    // cerr << "axis: " << axis << " range: " << range << "\n";
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
        // cerr << "\n";
        std::sort(objs.begin() + start, objs.begin() + end, comparator);
        // for (int i = start; i < end; i++) {
        //     vec3 v = objs[i]->bounding_box().centroid();
        //     cerr << v[0] << " " << v[1] << " " << v[2] << "\n";
        // }
        auto median_split = (max[axis] + min[axis]) / 2;
        // cerr << "\nmedian: " << median_split << "\n";
        double mid = start;
        for (int o = start; o < end; o++) {
            double curr = objs[o]->bounding_box().centroid()[axis];
            // cerr << "curr = " << curr << "\n";
            if (curr > median_split) {
                mid = fmax(mid, o);
                break;
            }
        }
        // cerr << "mid: " << mid << "\n\n\n\n\n\n\n\n";
        left = new bvh_node(objs, start, mid);
        right = new bvh_node(objs, mid, end);
    }

    aabb box_left = left->bounding_box();
    aabb box_right = right->bounding_box();
    
    // if (!left->bounding_box(box_left) || !right->bounding_box(box_right)) {
    //     std::cerr << "No bounding box in bvh constructor.\n";
    // }

    box = surrounding_box(box_left, box_right);
}

#endif