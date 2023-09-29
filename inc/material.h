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
        auto sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        auto direction = (sin_theta * refraction_ratio > 1.0) ?
                         reflect(unit_direction, rec.normal) :
                         refract(unit_direction, rec.normal, refraction_ratio);
        scattered = ray(rec.p, direction);
        return true;
    }
};