#pragma(once)

#include "vec.hpp"

class Ray
{
private:
    point3 orig;
    vec3 dir;
    Ray() = delete;
    Ray(const point3 &origin, const vec3 &direction)
    : orig(origin), dir(direction) {}

    point3 origin() const
    {
        return orig;
    }

    vec3 direction() const
    {
        return dir;
    }

    point3 at(double t) const
    {
        return orig + t * dir;
    }
};