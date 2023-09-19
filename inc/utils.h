#pragma once

#include <cmath>
#include <limits>
#include <memory>

// Usings
using std::vector;
using std::shared_ptr;
using std::sqrt;

// Constants
constexpr double inf = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;

// Helper functions
inline double deg_to_rad(double deg)
{
    return deg * PI / 180;
}

// Common headers
#include "vec.h"
#include "ray.h"
#include "color.h"