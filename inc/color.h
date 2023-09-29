#pragma once

#define cimg_display 0
#include "CImg.h"
#include "utils.h"

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

void write_color(cimg_library::CImg<unsigned char> &image, unsigned int x, unsigned int y, color c)
{
    static const interval intensity(0.000, 0.999);
    image(x, y, 0) = intensity.clamp(linear_to_gamma(c[0])) * 256;
    image(x, y, 1) = intensity.clamp(linear_to_gamma(c[1])) * 256;
    image(x, y, 2) = intensity.clamp(linear_to_gamma(c[2])) * 256;
}