#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "jitter.h"
#include "camera.h"

#include "objs.h"
#include "plane.h"
#include "sphere.h"
#include "triangle.h"

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::string;
using std::vector;


// --------------------------------------- VARIABLES --------------------------------------- //
static bool perspective = false;
static bool jittering = false;
static const int fine_grid = 16;
static int coarse_grid = (int) sqrt(fine_grid);

// Image
const static double aspect_ratio = 1.5 / 1.0;
const static int image_width = 100;
const static int image_height = static_cast<int>(image_width / aspect_ratio);

// Camera
const float viewport_width = 4.0;
const float s = viewport_width / image_width;
const vec3 direction = vec3(0, 0, -1);

// const point3 eyepoint = point3(-0.5, 1.0, 1);
const point3 eyepoint = point3(0,0,0);
const vec3 viewDir = point3(0, 0, -1);
const vec3 up = vec3(0,1,0);
double dir = 2.0;

const camera cam = camera(eyepoint, viewDir, up, dir, image_width, image_height, s);

// Colors
const color s1_c = color(91,75,122)/255.0;
const color s2_c = color(193,3,55)/255.0;
const color t1_c = color(0.0470446,0.678865,0.679296);
const color p_c = color(14, 153, 39)/255.0;
const color sky = color(0.5, 0.7, 1.0);

// Objects
const sphere s1 = sphere(point3(-0.2, 0, -1), 0.4, s1_c);
const sphere s2 = sphere(point3(0.4,0,-0.5), 0.1, s2_c);

const plane p = plane(point3(0, -1,0), vec3(0, -1, -0.1), p_c);

const vec3 a_1 = vec3(0.5, -0.8, -1);
const vec3 b_1 = vec3(-0.25, -1.0, -0.7);
const vec3 c_1 = vec3(0, 0.5, -0.7);
const triangle t1 = triangle(a_1, b_1, c_1, t1_c);

vector<const objs*> objects;

// Lighting and Shading
const vec3 lightPosition = vec3(0.75, 0.75, 0.5);

const vec3 kAmbient = vec3(1, 1, 1);
const vec3 iAmbient = vec3(0,0,0);

const vec3 iDiffuse = vec3(1,1,1);


// --------------------------------------- FUNCTIONS --------------------------------------- //

/**
 * The color at the position as determined by the Phong reflection model's diffuse shading
 * @param N surface normal
 * @param position the point we are trying to shade
 * @param kDiffuse base color for the object
 * @return shaded color
 */
color phong_reflection(vec3 N, point3 position, vec3 kDiffuse) {
    vec3 L = unit_vector(lightPosition - position); // light vector
    double diffuseLight = fmax(dot(L, N), 0.0);
    
    vec3 ambient = kAmbient * iAmbient;
    vec3 diffuse = kDiffuse * diffuseLight * iDiffuse;
    return ambient + diffuse;
}

/**
 * Generates a shadow ray for all hit points and determines if it will be in shadow or not.
 * The more objects covering it, the darker the shadow will be.
 * @param original the base color for the pixel
 * @param rec stores the hit point and its normal
 * @return the color based on if its in shadow
 */
color apply_shadows(color original, hit_record rec) {
    double epsilon = 0.0001;
    ray shadow_ray_before = ray(rec.p, lightPosition - rec.p);
    vec3 new_origin = shadow_ray_before.origin() + epsilon * shadow_ray_before.direction();
    ray shadow_ray = ray(new_origin, lightPosition - rec.p);
    hit_record tmp;
    color shadow = original;
    double hit; 
    int i = 0;
    for (auto o : objects) {
        if (i == 3) { // skip plane
            break;
        }
        hit = o->ray_intersection(shadow_ray, tmp);
        if (hit >= 0.0) {
            shadow = shade(shadow, 0.4);
        }
        i++;
    }
    return shadow;
}

/**
 * For each ray, determine what object is the closest and return the shaded color accordingly
 * @param r: the ray to shoot at all objects
 * @return the final color at the point after shading and shadows
 */
color ray_color(const ray& r) {
    hit_record rec;
    hit_record tmp;
    double hit;
    bool hit_object = false;
    double closest = std::numeric_limits<double>::infinity();

    for (auto o : objects) {
        hit = o->ray_intersection(r, tmp);
        if (hit >= 0.0 && hit <= closest) {
            hit_object = true;
            closest = tmp.t;
            rec = tmp;
        }
    }

    color to_return;

    if (hit_object) {
        to_return = phong_reflection(rec.normal, rec.p, rec.kD);
        to_return = apply_shadows(to_return, rec);
        return to_return;
    } 

    return sky;
}

/**
 * Calculates the center coordinate for the given pixel
 * @param i, j: the pixel coordinates in the image
 * @return the pixel center in the view plane
 */
vec3 get_pixel_center(int i, int j) {
    double x = s * (i - image_width / 2 + 0.5);
    double y = s * (j - image_height / 2 + 0.5);
    return vec3(x, y, 0);
}

/**
 * Calculates the sample coordinate within a single pixel
 * @param (i, j): the pixel coordinates in the image
 * @param (k, l): the sample position within the grid
 * @return a coordinate within the pixel in the view plane
 */
vec3 get_grid_pixel_center(int i, int j, int k, int l) {
    double delta_x = (k + 0.5) / fine_grid;
    double delta_y = (l + 0.5) / fine_grid;
    double x = s * (i - image_width / 2 + delta_x);
    double y = s * (j - image_height / 2 + delta_y);
    return vec3(x, y, 0);
}

/**
 * Shoots a single ray at the given point based on either perspective or orthographic projections
 * @param pixel_center the point of the pixel we are shooting through
 * @return the ray color based on the objects it hits
 */
color shoot_one_ray(vec3& pixel_center) {
    ray r;
    if (perspective) {
        r = cam.get_ray(pixel_center);
    } else {
        r = ray(pixel_center, direction);
    }
    return ray_color(r);
}

/**
 * Shoots multiple rays per pixel, using multi-jittered sampling
 * @param i, j: the pixel coordinates in the image
 * @return the average color for the pixel based of the different rays
 */
color shoot_multiple_rays(int i, int j) {
    bool** multi_jitter_mask = get_multi_jitter_mask(fine_grid);
    vector<color> colors;
    for (int k = 0; k < fine_grid; k++) {
        for (int l = 0; l < fine_grid; l++) {
            if (multi_jitter_mask[k][l]) {
                vec3 grid_center = get_grid_pixel_center(i, j, k, l);
                color pixel_color = shoot_one_ray(grid_center);
                colors.push_back(pixel_color);
            }
        }
    }
    return get_average_color(colors);
}

/**
 * Add the spheres, triangle, and plane into a list of objs
 */
void add_objects() {
    objects.push_back(&t1);
    objects.push_back(&s1);
    objects.push_back(&s2);
    objects.push_back(&p);
}

/**
 * Checks command line arguments for "p" and "j" to set perspective projection and jittering respectively
 */
void set_command_line_args(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!string(argv[i]).compare("p")) {
                perspective = true;
            }

            if (!string(argv[i]).compare("j")) {
                jittering = true;
            }
        }
    }
}

// Creates the objects and renders the scene with/without jittering in either perspective or orthographic
int main(int argc, char* argv[]) {
    set_command_line_args(argc, argv);
    add_objects();
    
    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >=0; j--) {
        cerr << "\rScanlines done: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            if (jittering) {
                color average = shoot_multiple_rays(i, j);
                write_color(cout, average);
            } else {
                vec3 pixel_center = get_pixel_center(i, j);
                color pixel_color = shoot_one_ray(pixel_center);
                write_color(cout, pixel_color);
            }
        }
    }

    cerr << "\nDone.\n";
}