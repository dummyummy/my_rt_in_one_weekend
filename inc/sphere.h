#pragma once

#include "hittable.h"

class sphere: public hittable
{
private:
    point3 center;
    double radius;

public:
    sphere(point3 center, double radius)
    : center(center), radius(radius) {}
    bool hit(const ray &ray, interval ray_t, hit_record &rec) const override
    {
        auto oc = ray.origin() - center;
        double a = dot(ray.direction(), ray.direction());
        // double b = 2 * dot(ray.direction(), oc);
        double h = dot(ray.direction(), oc); // b = 2 * h
        double c = dot(oc, oc) - radius * radius;
        double delta = h * h - a * c;
        if (delta < 0.0)
            return false;
        double sqrt_delta = sqrt(delta);
        auto root = (-h - sqrt_delta) / a;
        if (!ray_t.surrounds(root))
        {
            root = (-h + sqrt_delta) / a;
            if (!ray_t.surrounds(root))
                return false;
        }
        rec.p = ray.at(root);
        rec.set_face_normal(ray, (rec.p - center) / radius);
        rec.t = root;
        return true;
    }
};