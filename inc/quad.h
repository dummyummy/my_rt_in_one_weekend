#pragma once

#include "utils.h"
#include "hittable.h"
#include "material.h"
#include "hittable_list.h"

class quad : public hittable
{
private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D; // n.p=D;
    vec3 w;

public:
    quad(const point3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> m)
        : Q(Q), u(u), v(v), mat(m)
    {
        auto n = cross(u, v);
        normal = unit(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        set_bounding_box();
    }

    bool hit(const ray &ray, interval ray_t, hit_record &rec) const override
    {
        auto denom = dot(normal, ray.direction());

        // ray is parallel to the parallelogram
        if (fabs(denom) < 1e-8)
            return false;

        auto t = (D - dot(normal, ray.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        auto intersection = ray.at(t);
        auto planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        rec.p = intersection;
        rec.mat = mat;
        rec.t = t;
        rec.set_face_normal(ray, normal);

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    point3 sample() const override
    {
        return u * random_double() + v * random_double();
    }

    virtual void set_bounding_box()
    {
        bbox = aabb(Q, Q + u + v).pad();
    }

    virtual bool is_interior(double a, double b, hit_record &rec) const
    {
        if (a < 0 || 1 < a || b < 0 || 1 < b)
            return false;
        rec.u = a;
        rec.v = b;
        return true;
    }
};

inline shared_ptr<hittable_list> box(const point3 &a, const point3 &b, shared_ptr<material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<hittable_list>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

    return sides;
}