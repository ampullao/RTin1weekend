#ifndef MATERIAL_H
#define MATERIAL_H

#include "RTWeekend.h"
#include "texture.h"

class hit_record;

class material {
    public:
        virtual ~material() = default;

        virtual Color emitted(double u, double v, const Point3& p) const {
            return Color(0, 0, 0);
        }

        virtual bool scatter(
            const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};

class lambertian : public material {
    public:
        lambertian(const Color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        bool scatter(const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered)
        const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = Ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    private:
        shared_ptr<texture> albedo;
};

class metal: public material {
    public:
        metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        bool scatter(const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered)
        const override {
            Vec3 reflected = reflect(normalized(r_in.direction()), rec.normal);
            scattered = Ray(rec.p, reflected + fuzz * random_unit_vector(), r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    private:
        Color albedo;
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        bool scatter(const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered)
        const override {
            attenuation = Color(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

            Vec3 unit_direction = normalized(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            Vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = Ray(rec.p, direction, r_in.time());
            return true;
        }
    
    private:
        double ir; // Index of Refraction

        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
};

class diffuse_light : public material {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}
        diffuse_light(Color c) : emit(make_shared<solid_color>(c)) {}

        bool scatter(const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered)
        const override {
            return false;
        }

        Color emitted(double u, double v, const Point3& p) const override {
            return emit->value(u, v, p);
        }

    private:
        shared_ptr<texture> emit;
};

#endif