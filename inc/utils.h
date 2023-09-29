#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <random>

// Usings
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
constexpr double inf = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;

// Helper functions
inline double deg_to_rad(double deg)
{
    return deg * PI / 180;
}

inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 engine;
    return distribution(engine);
}

inline double random_double(double min, double max)
{
    return random_double() * (max - min) + min;
}

// Common headers
#include "vec.h"
#include "ray.h"
#include "interval.h"
#include "color.h"