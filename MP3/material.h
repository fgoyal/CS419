#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record; 

class material {
    public:
        virtual bool scatter(
            const ray& r, const hit_record& rec, color& c, ray& scattered
        ) const = 0;
};

class lambertian : public material {
    public: 
        lambertian(const color& a) : albedo(a) {}
        // virtual bool scatter(const ray& r, const hit_record& rec, color& c, ray& scattered) const;
        virtual bool scatter(const ray& r, const hit_record& rec, color& c, ray& scattered) const override {
            vec3 scatter_direction = rec.normal;
            scattered = ray(rec.p, scatter_direction);
            c = albedo;
            return true;
        }
    public:
        color albedo;
};

class metal : public material {
    public: 
        metal(const color& a) : albedo(a) {}
        virtual bool scatter(const ray& r, const hit_record& rec, color& c, ray& scattered) const override{
            vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
            scattered = ray(rec.p, reflected);
            c = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
    public:
        color albedo;
};




#endif