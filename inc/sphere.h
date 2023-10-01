#pragma once

#include "hittable.h"

class sphere: public hittable
{
private:
    point3 center1;
    vec3 center_vec;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;

    point3 center(double time) const noexcept
    {
        return center1 + time * center_vec;
    }

public:
    // stationary sphere
    sphere(point3 center1, double radius, shared_ptr<material> mat)
    : center1(center1), radius(radius), mat(mat), is_moving(false) {}
    // moving sphere
    sphere(point3 center1, point3 center2, double radius, shared_ptr<material> mat)
    : center1(center1), radius(radius), mat(mat), is_moving(true) { center_vec = center2 - center1; }
    bool hit(const ray &ray, interval ray_t, hit_record &rec) const override
    {
        point3 center = is_moving ? this->center(ray.time()) : center1;
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
        rec.mat = mat;
        return true;
    }
};