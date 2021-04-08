#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record; 

class material {
    public:
        virtual bool scatter(
            const ray& r, const hit_record& rec, ray& scattered
        ) const = 0;

        virtual color emitted() const {
            return color(0, 0, 0);
        }
};

class lambertian : public material {
    public: 
        virtual bool scatter(const ray& r, const hit_record& rec, ray& scattered) const override {
            vec3 scatter_direction = rec.normal + random_unit_vector();
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }
            scattered = ray(rec.p, scatter_direction);
            return true;
        }
};

class mirror : public material {
    public: 
        mirror(double f) : fuzz(f < 1 ? f : 1) {}
        virtual bool scatter(const ray& r, const hit_record& rec, ray& scattered) const override{
            vec3 reflected = reflect(r.direction(), rec.normal);
            scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
            return (dot(scattered.direction(), rec.normal) > 0);
        }
    public:
        double fuzz;
};

class glass : public material {
    public: 
        glass(double index) : ior(index) {}
        virtual bool scatter(const ray& r, const hit_record& rec, ray& scattered) const override{
            double refraction_ratio;
            vec3 n = rec.normal;
            if (dot(r.direction(), rec.normal) < 0) {
                refraction_ratio = 1.0 / ior;
            } else {
                refraction_ratio = ior;
            }

            vec3 unit_direction = r.direction();
            double cos_theta = fmin(dot(-unit_direction, n), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cant_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;
            if (cant_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
                direction = reflect(unit_direction, n);
            } else {
                direction = refract(unit_direction, n, refraction_ratio);
            }
            scattered = ray(rec.p, direction);
            return true;
        }

    public:
        double ior;
    
    private: 
        static double reflectance(double cosine, double ref_idx) {
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
};

class area_light : public material {
    public: 
        area_light(color emit_color) : c(emit_color) {}
        virtual bool scatter(const ray& r, const hit_record& rec, ray& scattered) const override{
            return false;
        }
        virtual color emitted() const override {
            return c;
        }
    public:
        color c;
};




#endif