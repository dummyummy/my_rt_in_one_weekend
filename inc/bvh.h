#pragma once

#include "utils.h"
#include "hittable.h"
#include "hittable_list.h"

#include <functional>
#include <tuple>


class bvh_node : public hittable
{
private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

public:
    bvh_node(const hittable_list& list)
    : bvh_node(list.get_objects(), 0, list.get_objects().size()) {}

    bvh_node(const std::vector<shared_ptr<hittable>> &src_objects, size_t start, size_t end)
    {
        static auto comp_func = [](const shared_ptr<hittable> a, const shared_ptr<hittable> b, int ax) -> bool
        {
            return a->bound_box().axis(ax).min < b->bound_box().axis(ax).min;
        };
        static auto comp_x = std::bind(comp_func, std::placeholders::_1, std::placeholders::_2, 0);
        static auto comp_y = std::bind(comp_func, std::placeholders::_1, std::placeholders::_2, 1);
        static auto comp_z = std::bind(comp_func, std::placeholders::_1, std::placeholders::_2, 2);
        static auto comps = std::vector<decltype(comp_x)>({comp_x, comp_y, comp_z});

        std::vector<shared_ptr<hittable>> objects(src_objects.begin() + start, src_objects.begin() + end);
        int ax = random_int(0, 2);
        size_t object_span = end - start;
        if (object_span == 1)
            left = right = objects[0];
        else if (object_span == 2)
        {
            if (comps[ax](objects[0], objects[1]))
            {
                left = objects[0];
                right = objects[1];
            }
            else
            {
                left = objects[1];
                right = objects[0];
            }
        }
        else
        {
            std::sort(objects.begin(), objects.begin() + object_span, comps[ax]);
            auto mid = object_span / 2;
            left = make_shared<bvh_node>(objects, 0, mid);
            right = make_shared<bvh_node>(objects, mid, object_span);
        }
        bbox = aabb(left->bound_box(), right->bound_box());
    }

    bool hit(const ray &ray, interval ray_t, hit_record &rec) const
    {
        if (!bbox.hit(ray, ray_t))
            return false;
        bool hit_left = left->hit(ray, ray_t, rec);
        bool hit_right = right->hit(ray, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bound_box() const
    {
        return bbox;
    }
};

using bvh = bvh_node;