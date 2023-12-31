#pragma once

#include "utils.h"
#include "hittable.h"
#include "texture.h"

class material
{
protected:
    bool emitting_flag = false;
public:
    virtual ~material() = default;
    virtual color emitted(double u, double v, const point3 &p) const
    {
        return color(0, 0, 0);
    }
    virtual bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
    virtual bool is_emitting() const
    {
        return emitting_flag;
    }
};

class lambertian: public material
{
private:
    shared_ptr<texture> albedo = nullptr;

public:
    lambertian(const color &a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(const shared_ptr<texture> &tex) : albedo(tex) {}

    bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
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
        scattered = ray(rec.p, reflected + fuzziness * random_unit_vector(), r_in.time());
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
        scattered = ray(rec.p, direction, r_in.time());
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

class diffuse_light: public material
{
private:
    shared_ptr<texture> emit;
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) { emitting_flag = true; }
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) { emitting_flag = true; }

    bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        return false;
    }

    color emitted(double u, double v, const point3 &p) const override
    {
        return emit->value(u, v, p);
    }
};

class isotropic : public material
{
private:
    shared_ptr<texture> albedo;

public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    bool scattered(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
};