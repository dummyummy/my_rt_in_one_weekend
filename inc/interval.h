#pragma once

#include "utils.h"

class interval
{
public:
    double min, max;

    interval()
    : min(+inf), max(-inf) {} // Empty by default

    interval(double min, double max)
    : min(min), max(max) {}

    interval(const interval &a, const interval &b)
    : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    double clamp(double x) const
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    double size() const
    {
        return max - min;
    }

    interval expand(double delta) const
    {
        auto padding = delta / 2;
        return interval(min - padding, max + padding);
    }

    const static interval empty;
    const static interval universe;
};

const interval interval::empty(inf, -inf);
const interval interval::universe(-inf, inf);