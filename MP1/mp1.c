#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"
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
const sphere s1 = sphere(point3(0, 0, -1), 0.5);
const sphere s2 = sphere(point3(-1, 0, -1), 0.5);

const point3 plane_point = point3(0, 0.1, 0);
const vec3 plane_normal = vec3(0, -1, 0);

// ----------------- PHONG REFLECTION MODEL --------------------- //
const vec3 lightPosition = vec3(0.5, -0.5, 0);

// Ambient Lighting
const vec3 kAmbient = vec3(1, 1, 1);
const vec3 iAmbient = vec3(0,0,0);

// Diffuse Lighting
const vec3 kDiffuse = vec3(255, 230, 0)/255.0;
const vec3 iDiffuse = vec3(1, 1, 1);

/**
 * @param center
 * @param radius
 * @param r
 * @return
 */
double sphere_intersection(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant)) / (a);
    }
}

double sphere_intersection(const sphere& sphere, const ray& r) {
    vec3 oc = r.origin() - sphere.center();
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - sphere.radius() * sphere.radius();
    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant)) / (a);
    }
}

double plane_intersection(const point3& a, const vec3& n, const ray& r) {
    auto t = dot((a - r.origin()), n) / dot(r.direction(), n);
    return t;
}

color phong_reflection(vec3 N, point3 position) {
    vec3 L = unit_vector(lightPosition - position); // light vector
    double diffuseLight = fmax(dot(L, N), 0.0);
    
    vec3 ambient = kAmbient * iAmbient;
    vec3 diffuse = kDiffuse * diffuseLight * iDiffuse;
    return ambient + diffuse;
}

color ray_color(const ray& r) {
    double hit = ray_sphere_intersection(s1, r);
    if (hit >= 0.0) {
        point3 position = r.at(hit);
        vec3 N = s1.surface_normal(position);
        return phong_reflection(N, position);
    }
    hit = ray_sphere_intersection(s2, r);
    if (hit >= 0.0) {
        point3 position = r.at(hit);
        vec3 N = s2.surface_normal(position);
        return phong_reflection(N, position);
    }
    hit = plane_intersection(plane_point, plane_normal, r);
    if (hit >= 0.0) {
        // return color(14, 153, 39)/255.0;
        return phong_reflection(plane_normal, r.at(hit));
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