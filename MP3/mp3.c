#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "jitter.h"
#include "camera.h"
#include "utils.h"
#include "mesh.h"
#include "material.h"

#include "objs.h"
#include "plane.h"
#include "sphere.h"
#include "rectangle.h"
#include "triangle.h"
#include "aabb.h"
#include "bvh_node.h"

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <limits>

using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::numeric_limits;


// --------------------------------------- VARIABLES --------------------------------------- //
static bool perspective = false;
static bool jittering = false;
static const int fine_grid = 16;
static int coarse_grid = (int) sqrt(fine_grid);
const int max_depth = 50;
double infinity = numeric_limits<double>::infinity();

// Image
const static double aspect_ratio = 1.0 / 1.0;
const static int image_width = 150;
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
const color purple = color(91,75,122)/255.0;
const color orange = color(219, 121, 59)/255.0;
const color pink = color(0.7, 0.3, 0.3);
const color white = color(1.0, 1.0, 1.0);
const color blue = color(0.0470446,0.678865,0.679296);
const color yellow = color(0.8, 0.8, 0.0);
const color sky = color(0.5, 0.7, 1.0);
const color black = color(0.1, 0.1, 0.1);
const color gray = color(0.8, 0.8, 0.8);
const color bright_white = color(4, 4, 4);

// Objects
const int NUM_OBJECTS = 10;
const double sphere_radius = 0.5;
vector<objs*> objects;
bvh_node root;

sphere* s1 = new sphere(point3(-0.2, -0.3,   -1), 0.3,  gray, new mirror(0.05));
sphere* s2 = new sphere(point3( 0.4, -0.2,   -1), 0.3,  white, new glass(1.5));
sphere* s3 = new sphere(point3( 0.3, -0.43, -0.7), 0.07, pink, new lambertian());
sphere* light = new sphere(point3(-0.2, 0.7, -1), 0.3,  bright_white, new area_light(bright_white));
// rectangle* r1 = new rectangle(-0.5, 0.5, -0.5, -1, 0, bright_white, new lambertian());
plane* p = new plane(point3(0, -0.5,0), vec3(0, -1, 0), white, new lambertian());
const vec3 a_1 = vec3(-0.3, -0.8, -0.5); // front
const vec3 b_1 = vec3(-0.8, -0.6, -1); // back
const vec3 c_1 = vec3(-0.4, 0.2, -0.7); // top
triangle* t1 = new triangle(a_1, b_1, c_1, blue, new lambertian());

const vec3 a_2 = vec3(-1, -0.2, -1.5); // front
const vec3 b_2 = vec3(1, -0.2, -1.5); // back
const vec3 c_2 = vec3(1, 1, -1.5); // top
const vec3 d_2 = vec3(-1, 1, -1.5);
triangle* r1 = new triangle(a_2, b_2, c_2, white, new area_light(white));
triangle* r2 = new triangle(a_2, c_2, d_2, white, new area_light(white));

const vec3 a_3 = vec3(-1, -0.49, -0.5); // front
const vec3 b_3 = vec3(1, -0.49, -0.5); // back
const vec3 c_3 = vec3(0, -0.49, -1.5); // top
triangle* r3 = new triangle(c_3, b_3, a_3, white, new area_light(white));

// Lighting and Shading
const vec3 lightPosition = vec3(0.75, 0.75, 0.5);

const vec3 kAmbient = vec3(1, 1, 1);
const vec3 iAmbient = vec3(0,0,0);

const vec3 iDiffuse = vec3(1,1,1);

const vec3 kSpecular = vec3(1,1,1);
const vec3 iSpecular = vec3(1,1,1);
const float shininess = 20;

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
    vec3 V = unit_vector(eyepoint - position);
    vec3 R = unit_vector(reflect(L, N));
    // vec3 R = unit_vector(2 * dot(L, N) * N - L);
    double diffuseLight = fmax(dot(L, N), 0.0);
    double specularLight = fmax(pow(dot(R, V), shininess), 0.0);

    vec3 ambient = kAmbient * iAmbient;
    vec3 diffuse = kDiffuse * diffuseLight * iDiffuse;
    vec3 specular = kSpecular * specularLight * iSpecular;
    // color c = ambient + diffuse + specular;
    color c = ambient + diffuse;
    return vec_clamp(c, 0.0, 1.0);
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
    int i = 0;
    // bool hit = root.ray_intersection(shadow_ray, tmp, 0.001, infinity);
    // if (hit) {
    //     shadow = shade(shadow, 0.4);
    // }
    for (auto o : objects) {
        if (i == 4) { // skip plane
            break;
        }
        bool hit = o->ray_intersection(shadow_ray, tmp, 0.001, infinity);
        if (hit) {
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
color ray_color(const ray& r, int depth) {
    if (depth <= 0) {
        return black;
    }

    hit_record rec;
    bool hit = root.ray_intersection(r, rec, 0.0001, infinity);

    // hit_record rec;
    // hit_record tmp;
    // bool hit;
    // bool hit_object = false;
    // double closest = std::numeric_limits<double>::infinity();

    // for (auto o : objects) {
    //     hit = o->ray_intersection(r, tmp, 0.001, infinity);
    //     if (hit && tmp.t <= closest) {
    //         hit_object = true;
    //         closest = tmp.t;
    //         rec = tmp;
    //     }
    // }
    color to_return;
    // cerr << hit << " " << rec.t << " (" << rec.kD << ")\n";
    // if (hit_object) {
    if (hit) {
        ray scattered;
        color emitted = rec.mat->emitted();
        // cerr << emitted << "\n";
        if (rec.mat->scatter(r, rec, scattered)) {
            to_return = emitted + rec.kD * ray_color(scattered, depth - 1);
        } else {
            return emitted;
        }
        // cerr << rec.normal << "\n";
        // return color(rec.normal.z(), rec.normal.z(), rec.normal.z());
        // return 0.5 * color(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
        // to_return = phong_reflection(rec.normal, rec.p, rec.kD);
        // to_return = apply_shadows(to_return, rec);
        return to_return;
    }  
    return sky;
    // return black;
    // return color(0.1, 0.1, 0.1);
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
    return ray_color(r, max_depth);
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
    objects.push_back(t1);
    objects.push_back(s1);
    objects.push_back(s2);
    objects.push_back(s3);
    // objects.push_back(light);
    objects.push_back(r1);
    objects.push_back(r2);
    objects.push_back(r3);
    // objects.push_back(p);

    // for (int i = 0; i < NUM_OBJECTS; i++) {
    //     point3 center = random_sphere();
    //     color c = random_vec3(0.0, 1.0);
    //     sphere* randsphere = new sphere(center, sphere_radius, c, s1_m);
    //     objects.push_back(randsphere);
    // }
    // cerr << "created object list\n";
    cerr << "OBJECTS SIZE: " << objects.size() << "\n";
    for (auto o : objects) {
        cerr << o->type() << ": " << o->kDiffuse() << "\n";
    }
    root = bvh_node(objects);
    // cerr << "root bbox: " << root.bounding_box() << "\n";
}

/**
 * Create a mesh given the obj file, create the BVH tree for it, and store it in root
 */
void create_mesh() {
    color obj_color = color(1,0,0);
    mesh obj = mesh("objs/cow.obj", obj_color, new lambertian());
    vector<objs*> mesh = obj.get_faces();
    root = bvh_node(mesh);
    cerr << "created root\n";
}

void generate_checkerboard(double y, color color_a, color color_b) {
    vec3 a;
    vec3 b;
    vec3 c;
    vec3 d;
    double width = 1;
    double length = 1;
    for (double z = 0; z > -10; z -= length) {
        for (double x = -10; x < 10; x += width) {
            a = vec3(x, y, z - length);
            b = vec3(x, y, z);
            c = vec3(x + width, y, z);
            d = vec3(x + width, y, z - length);
            objects.push_back(new triangle(a, b, c, color_a, new lambertian()));
            objects.push_back(new triangle(a, c, d, color_b, new lambertian()));
        }
    }
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
    std::clock_t start;
    double duration;
    start = std::clock();

    srand(time(NULL));
    set_command_line_args(argc, argv);

    generate_checkerboard(-0.5, color(255, 228, 156)/255.0, color(0.3,0.3,0.3));
    add_objects(); // for spheres
    
    // create_mesh();
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    cerr << "\nduration to construct tree is: " << duration << "\n";

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

    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    cerr << "\nduration with " << objects.size() << " objects is: " << duration << "\n";

    cerr << "\nDone.\n";
}