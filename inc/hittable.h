#pragma once

#include "utils.h"
#include "aabb.h"

class material;

class hit_record
{
public:
    point3 p;
    vec3 normal; // must be unit vector
    shared_ptr<material> mat;
    double t;
    bool front_face;
    double u, v; // texture coordinates

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
    virtual aabb bounding_box() const = 0;
    virtual point3 sample() const
    {
        return point3(0, 0, 0);
    }
};

class translate : public hittable
{
private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;

public:
    translate(shared_ptr<hittable> p, const vec3 &displacement)
        : object(p), offset(displacement)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        ray offset_ray(r.origin() - offset, r.direction(), r.time());

        if (!object->hit(offset_ray, ray_t, rec))
            return false;

        rec.p += offset;
        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }
};

class rotate_y : public hittable
{
private:
    shared_ptr<hittable> object;
    aabb bbox;
    double sin_theta;
    double cos_theta;

public:
    rotate_y(shared_ptr<hittable> p, double angle) : object(p)
    {
        auto rad = deg_to_rad(angle);
        sin_theta = sin(rad);
        cos_theta = cos(rad);
        bbox = object->bounding_box();

        point3 min(inf, inf, inf);
        point3 max(-inf, -inf, -inf);
        for (int i = 0; i <= 1; i++)
        {
            auto x = (1 - i) * bbox.x.min + i * bbox.x.max;
            for (int j = 0; j <= 1; j++)
            {
                auto y = (1 - j) * bbox.y.min + j * bbox.y.max;
                for (int k = 0; k <= 1; k++)
                {
                    auto z = (1 - k) * bbox.z.min + k * bbox.z.max;

                    auto new_x = cos_theta * x + sin_theta * z;
                    auto new_z = -sin_theta * x + cos_theta * z;

                    vec3 tester(new_x, y, new_z);

                    for (int ax = 0; ax < 3; ax++)
                    {
                        min[ax] = fmin(min[ax], tester[ax]);
                        max[ax] = fmax(max[ax], tester[ax]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin().x() - sin_theta * r.origin().z();
        origin[2] = sin_theta * r.origin().x() + cos_theta * r.origin().z();

        direction[0] = cos_theta * r.direction().x() - sin_theta * r.direction().z();
        direction[2] = sin_theta * r.direction().x() + cos_theta * r.direction().z();

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.p;
        p[0] = cos_theta * rec.p.x() + sin_theta * rec.p.z();
        p[2] = -sin_theta * rec.p.x() + cos_theta * rec.p.z();
        rec.p = p;

        auto normal = rec.normal;
        normal[0] = cos_theta * rec.normal.x() + sin_theta * rec.normal.z();
        normal[2] = -sin_theta * rec.normal.x() + cos_theta * rec.normal.z();
        rec.normal = normal;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }
};