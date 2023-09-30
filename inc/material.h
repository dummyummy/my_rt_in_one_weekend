#pragma once

#include "utils.h"
#include "hittable.h"

class material
{
public:
    virtual ~material() = default;

    virtual bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
};

class lambertian: public material
{
private:
    color albedo;

public:
    lambertian(const color &a) : albedo(a) {}

    bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

class metal: public material
{
private:
    color albedo;
    double fuzziness; // glossy reflection

public:
    metal(const color &a, double f = 0.0) : albedo(a), fuzziness(f) {}

    bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        auto reflected = reflect(unit(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzziness * random_unit_vector());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0.0;
    }
};

class dielectric: public material
{
private:
    double ir; // index of refraction

public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
        auto unit_direction = unit(r_in.direction());
        auto cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double reflectance = schlick(cos_theta, refraction_ratio);
        vec3 direction;
        if (reflectance > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        scattered = ray(rec.p, direction);
        return true;
    }

private:
    static double schlick(double cosine, double ref_idx) // Schlick Approximation
    {
        // if ref_idx is greater than 1(from dense to sparse), use refract angle for cosine
        auto f0 = (ref_idx - 1) / (ref_idx + 1);
        f0 = f0 * f0;
        if (ref_idx > 1.0)
        {
            double cosine2 = 1.0 - ref_idx * ref_idx * (1.0 - cosine * cosine);
            if (cosine2 < 0.0)
                return 1.0;
            cosine = sqrt(cosine2);
        }
        return f0 + (1 - f0) * pow(1 - cosine, 5);
    }
};