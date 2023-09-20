#pragma once

#include <vector>
#include <memory>

#include "utils.h"
#include "hittable.h"

class hittable_list : public hittable
{
private:
    vector<shared_ptr<hittable>> objects;
public:
    hittable_list() = default;
    hittable_list(const vector<std::shared_ptr<hittable>> &objects)
    : objects(objects) {}
    hittable_list(const shared_ptr<hittable> &object)
    {
        add(object);
    }

    void clear()
    {
        objects.clear();
    }

    void add(const std::shared_ptr<hittable> &object)
    {
        objects.push_back(object);
    }

    bool hit(const ray &ray, interval ray_t, hit_record &rec) const override
    {
        hit_record temp_rec;
        bool hit_anything = false;
        double tmax_so_far = ray_t.max;

        for (auto &object : objects)
        {
            if (object->hit(ray, interval(ray_t.min, tmax_so_far), temp_rec))
            {
                hit_anything = true;
                tmax_so_far = temp_rec.t;
            }
        }

        if (hit_anything)
            rec = temp_rec;
        return hit_anything;
    }
};