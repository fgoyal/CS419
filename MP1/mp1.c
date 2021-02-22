#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "objs.h"
#include <iostream>

static bool perspective = false;

// Image
const static double aspect_ratio = 1.5 / 1.0;
const static int image_width = 500;
const static int image_height = static_cast<int>(image_width / aspect_ratio);

// Camera
const float viewport_width = 4.0;
const float viewport_height = viewport_width / aspect_ratio;
const float focal_length = 1.0;

// Perspective
const point3 origin = point3(0,0,0);
const vec3 horizontal = vec3(viewport_width, 0, 0);
const vec3 vertical = vec3(0, viewport_height, 0);
const vec3 lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

// Orthographic
const float s = viewport_width / image_width;
const vec3 direction = vec3(0, 0, -1);

// Shapes
const sphere s1 = sphere(point3(0, 0, -1), 0.5, get_random_color());
const sphere s2 = sphere(point3(-1, 0, -1), 0.5, get_random_color());

const plane p = plane(point3(0, 0.5, -1), vec3(0, -1, 0));

const vec3 a_1 = vec3(1, 0.5, -1);
const vec3 b_1 = vec3(0.3, 0.5, -0.5);
const vec3 c_1 = vec3(0, -0.5, -0.5);

const triangle t1 = triangle(a_1, b_1, c_1);

// ----------------- PHONG REFLECTION MODEL --------------------- //
const vec3 lightPosition = vec3(0.5, -1, 1);

// Ambient Lighting
const vec3 kAmbient = vec3(1, 1, 1);
const vec3 iAmbient = vec3(0,0,0);

// Diffuse Lighting
const vec3 iDiffuse = vec3(1, 1, 1);

color phong_reflection(vec3 N, point3 position, vec3 kDiffuse) {
    vec3 L = unit_vector(lightPosition - position); // light vector
    double diffuseLight = fmax(dot(L, N), 0.0);
    
    vec3 ambient = kAmbient * iAmbient;
    vec3 diffuse = kDiffuse * diffuseLight * iDiffuse;
    return ambient + diffuse;
}

color ray_color(const ray& r) {
    hit_record rec;
    double hit = s1.ray_intersection(r, rec);
    if (hit >= 0.0) {
        point3 position = r.at(hit);
        vec3 N = s1.surface_normal(position);
        return phong_reflection(N, position, s1.kDiffuse());
    }
    hit = s2.ray_intersection(r, rec);
    if (hit >= 0.0) {
        point3 position = r.at(hit);
        vec3 N = s2.surface_normal(position);
        return phong_reflection(N, position, s2.kDiffuse());
    }
    hit = t1.ray_intersection(r, rec);
    if (hit >= 0.0) {
        // return color(1, 1, 1);
        // std::cerr << t1.normal() << "\n";
        return phong_reflection(t1.normal(), r.at(hit), color(1,1,1));
    }
    hit = p.ray_intersection(r, rec);
    if (hit >= 0.0) {
        // std::cerr << r.at(hit) << "\n";
        return color(14, 153, 39)/255.0;
        // return phong_reflection(p.normal(), r.at(hit), color(14, 153, 39)/255.0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    hit = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - hit) * color(1.0, 1.0, 1.0) + hit * color(0.5, 0.7, 1.0);
}

ray get_ray_perspective(int i, int j) {
    double u = double(i) / (image_width - 1);
    double v = double(j) / (image_height - 1);
    vec3 pixel_center = lower_left_corner + u * horizontal + v * vertical;
    // std::cerr << pixel_center << "\n";
    return ray(origin, pixel_center - origin);
}

ray get_ray_orthographic(int i, int j) {
    double x = s * (i - image_width / 2 + 0.5);
    double y = s * (j - image_height / 2 + 0.5);
    vec3 pixel_center = vec3(x, y, 0);
    // std::cerr << pixel_center << "\n";
    return ray(pixel_center, direction);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (!std::string(argv[1]).compare("-p")) {
            perspective = true;
        }
    }
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        std::cerr << "\rScanlines done: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            // std::cerr << "(i, j) = " << i << " " << j << "\n";
            ray r;
            if (perspective) {
                r = get_ray_perspective(i, j);
            } else {
                r = get_ray_orthographic(i, j);
            }
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}