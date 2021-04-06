#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record; 

class material {
    public:
        virtual bool scatter(
            const ray& r, const hit_record& rec, ray& scattered
        ) const = 0;
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
        virtual bool scatter(const ray& r, const hit_record& rec, ray& scattered) const override{
            vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
            scattered = ray(rec.p, reflected);
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

class glass : public material {
    public: 
        glass(double index) : ior(index) {}
        virtual bool scatter(const ray& r, const hit_record& rec, ray& scattered) const override{
            double refraction_ratio;
            if (dot(r.direction(), rec.normal) < 0) {
                refraction_ratio = 1.0 / ior;
            } else {
                refraction_ratio = ior;
            }

            vec3 unit_direction = unit_vector(r.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cant_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cant_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
                direction = reflect(unit_direction, rec.normal);
            } else {
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            }
            // vec3 refracted = refract(unit_vector(r.direction()), rec.normal, refraction_ratio);
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




#endif