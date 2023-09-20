#pragma once

#include "ray.h"

class hit_record
{
public:
    point3 p;
    vec3 normal; // must be unit vector
    double t;
    bool front_face;

    /**
     * @param outward_normal must be a unit vector
     */
    void set_face_normal(const ray &ray, const vec3 &outward_normal)
    {
        front_face = (dot(ray.direction(), outward_normal) < 0.0);
        normal = (front_face ? outward_normal : -outward_normal);
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &ray, interval ray_t, hit_record &rec) const = 0;
};