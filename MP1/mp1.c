#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "objs.h"
#include <iostream>
#include <vector>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <random>

// ----------------- VARIABLES --------------------- //
static bool perspective = false;
static bool jittering = true;
static const int fine_grid = 16;
static int coarse_grid = (int) sqrt(fine_grid);
// bool** multi_jitter_mask;

// Image
const static double aspect_ratio = 1.5 / 1.0;
const static int image_width = 500;
const static int image_height = static_cast<int>(image_width / aspect_ratio);

// Camera
const float viewport_width = 4.0;
const float viewport_height = viewport_width / aspect_ratio;
const float focal_length = 1.0;
const point3 origin = point3(0,0,0);

const float s = viewport_width / image_width;
const vec3 direction = vec3(0, 0, -1);

// Colors
const color s1_c = color(91,75,122)/255.0;
const color s2_c = color(193,3,55)/255.0;
const color t1_c = color(0.0470446,0.678865,0.679296);
const color p_c = color(14, 153, 39)/255.0;
const color sky = color(0.5, 0.7, 1.0);

// Objects
const sphere s1 = sphere(point3(0, 0, -1), 0.4, s1_c);
const sphere s2 = sphere(point3(0, -1, -1), 0.3, s2_c);

const plane p = plane(point3(0, 0.5, -1), vec3(0, -0.5, 0.5), p_c);

const vec3 a_1 = vec3(1, 0.5, -0.8);
const vec3 b_1 = vec3(0.25, 0.5, -0.5);
const vec3 c_1 = vec3(0.25, -0.5, -0.8);

const triangle t1 = triangle(a_1, b_1, c_1, t1_c);

std::vector<const objs*> objects;
// ----------------- PHONG REFLECTION MODEL --------------------- //
const vec3 lightPosition = vec3(0.75, -0.75, 0.5);

// Ambient Lighting
const vec3 kAmbient = vec3(1, 1, 1);
const vec3 iAmbient = vec3(0,0,0);

// Diffuse Lighting
const vec3 iDiffuse = vec3(1,1,1);
// const vec3 iDiffuse_shadow = vec3(0.5, 0.5, 0.5);


// ----------------- FUNCTIONS --------------------- //

color phong_reflection(vec3 N, point3 position, vec3 kDiffuse) {
    vec3 L = unit_vector(lightPosition - position); // light vector
    double diffuseLight = fmax(dot(L, N), 0.0);
    
    vec3 ambient = kAmbient * iAmbient;
    vec3 diffuse = kDiffuse * diffuseLight * iDiffuse;
    return ambient + diffuse;
}

color apply_shadows(color original, hit_record rec) {
    float e = 0.00001;
    ray shadow_ray = ray(rec.p, lightPosition - rec.p);
    hit_record tmp;
    color shadow = original;
    double hit; 
    int i = 0;
    for (auto o : objects) {
        if (i == 3) {
            break;
        }
        hit = o->ray_intersection(shadow_ray, tmp);
        if (hit >= 0.0) {
            shadow = shade(shadow, 0.4);
        }
        i++;
    }
    // std::cerr << "no hit\n";
    return shadow;
}

color ray_color(const ray& r) {
    hit_record rec;
    hit_record tmp;
    double hit;
    bool hit_object = false;
    double closest = std::numeric_limits<double>::infinity();
    bool sphere1 = false;
    int i = 0;
    for (auto o : objects) {
        hit = o->ray_intersection(r, tmp);
        if (hit >= 0.0 && hit <= closest) {
            hit_object = true;
            closest = tmp.t;
            rec = tmp;
            if (i == 0) {
                sphere1 = true;
            }
        }
        i++;
    }

    color to_return;

    if (hit_object) {
        // bool shadow = in_shadow(rec);
        to_return = phong_reflection(rec.normal, rec.p, rec.kD);
        to_return = apply_shadows(to_return, rec);
        return to_return;
    } 

    // vec3 unit_direction = unit_vector(r.direction());
    // hit = 0.5 * (unit_direction.y() + 1.0);
    // return (1.0 - hit) * color(1.0, 1.0, 1.0) + hit * sky;
    return sky;
}

vec3 get_pixel_center(int i, int j) {
    double x = s * (i - image_width / 2);
    double y = s * (j - image_height / 2);
    return vec3(x, y, 0);
}

vec3 get_grid_pixel_center(int i, int j, int k, int l) {
    double x = s * (i - image_width / 2);
    double y = s * (j - image_height / 2);
    x = x + ((k + 0.5) * s / fine_grid);
    y = y + ((l + 0.5) * s / fine_grid);
    return vec3(x, y, 0);
}

void add_objects() {
    objects.push_back(&t1);
    objects.push_back(&s1);
    objects.push_back(&s2);
    objects.push_back(&p);
}

int main(int argc, char* argv[]) {
    
    if (argc > 1) {
        if (!std::string(argv[1]).compare("-p")) {
            perspective = true;
        }
    }
    add_objects();
    std::cerr << "s = " << s << "\n";
    // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // srand(time(NULL));
    // // for (int i = 0; i < 10; i++) {
    // //     auto r = random_int(0,4);
    // //     // auto r = rand() % fine_grid;
    // //     std::cerr << r << "\n";
    // // }
    // if (jittering) {
    //     bool** sample = get_multi_jitter_mask(fine_grid);
        
    //     for (int j = 0; j < fine_grid; ++j) {
    //         std::cerr << "\rScanlines done: " << j << ' ' << std::flush;
    //         for (int i = 0; i < fine_grid; ++i) {
    //             if (sample[i][j]) {
    //                 write_color(std::cout, color(0,0,0));
    //             } else {
    //                 write_color(std::cout, color(1,1,1));
    //             }
    //         }
    //     }
    // }
    // return 0;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        std::cerr << "\rScanlines done: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            if (jittering) {
                bool** multi_jitter_mask = get_multi_jitter_mask(fine_grid);
                std::vector<color> pixel;
                for (int k = 0; k < fine_grid; k++) {
                    for (int l = 0; l < fine_grid; l++) {
                        if (multi_jitter_mask[k][l]) {
                            vec3 grid_center = get_grid_pixel_center(i, j, k, l);
                            ray r;
                            if (perspective) {
                                r = ray(origin, grid_center - origin - vec3(0, 0, focal_length));
                            } else {
                                r = ray(grid_center, direction);
                            }
                            // std::cerr << grid_center << "\n";
                            color pixel_color = ray_color(r);
                            pixel.push_back(pixel_color);
                        }
                    }
                }
                color average = get_average_color(pixel);
                write_color(std::cout, average);
                // std::cerr << pixel.size() << "\n";
            } else {
                // std::cerr << "(i, j) = " << i << " " << j << "\n";
                ray r;
                vec3 pixel_center = get_pixel_center(i, j);
                // std::cerr << pixel_center << "\n";
                // vec3 grid_center = get_grid_pixel_center(i, j);
                if (perspective) {
                    r = ray(origin, pixel_center - origin - vec3(0, 0, focal_length));
                } else {
                    r = ray(pixel_center, direction);
                }
                color pixel_color = ray_color(r);
                // std::cerr << pixel_color << "\n";
                write_color(std::cout, pixel_color);
            }
        }
        // std::cerr << "\n";
    }

    std::cerr << "\nDone.\n";
}