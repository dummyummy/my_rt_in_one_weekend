#pragma once

#include "utils.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
        : boundary(b), neg_inv_density(-1 / d), phase_function(make_shared<isotropic>(a))
    {
    }

    constant_medium(shared_ptr<hittable> b, double d, color c)
        : boundary(b), neg_inv_density(-1 / d), phase_function(make_shared<isotropic>(c))
    {
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        hit_record rec1, rec2;

        // set interval to universe in case origin is inside the boundary
        if (!boundary->hit(r, interval::universe, rec1))
            return false;
        if (!boundary->hit(r, interval(rec1.t + 0.0001, inf), rec2))
            return false;
        
        // interval intersection
        if (rec1.t < ray_t.min)
            rec1.t = ray_t.min;
        if (rec2.t > ray_t.max)
            rec2.t = ray_t.max;
        
        if (rec1.t >= rec2.t)
            return false;
        
        if (rec1.t < 0)
            return false;

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * log(random_double());

        if (hit_distance > distance_inside_boundary)
            return false;
        
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = random_unit_vector();
        rec.front_face = true;
        rec.mat = phase_function;
        return true;
    }

    aabb bounding_box() const override
    {
        return boundary->bounding_box();
    }

private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function; // material of subsurface
};