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
    aabb bbox;

    point3 center(double time) const noexcept
    {
        return center1 + time * center_vec;
    }

    /**
     * @brief calculate uv coordinates for a given point
     * @param p point to be calculated, must be on the unit sphere centerd at origin
     * @param u returned value [0, 1] of angle around the Y axis from the -X axis
     * @param v returned value [0, 1] of angle from the -Y axis
     */
    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        static double inv_pi = 1.0 / PI;
        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + PI;

        u = phi * 0.5 * inv_pi;
        v = theta * inv_pi;
    }

public:
    // stationary sphere
    sphere(point3 center1, double radius, shared_ptr<material> mat)
    : center1(center1), radius(radius), mat(mat), is_moving(false)
    {
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }
    // moving sphere
    sphere(point3 center1, point3 center2, double radius, shared_ptr<material> mat)
    : center1(center1), radius(radius), mat(mat), is_moving(true)
    {
        auto rvec = vec3(radius, radius, radius);
        aabb box1 = aabb(center1 - rvec, center1 + rvec);
        aabb box2 = aabb(center2 - rvec, center2 + rvec);
        bbox = aabb(box1, box2);
        center_vec = center2 - center1;
    }
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
        auto outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(ray, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.t = root;
        rec.mat = mat;
        return true;
    }

    aabb bounding_box() const
    {
        return bbox;
    }
};