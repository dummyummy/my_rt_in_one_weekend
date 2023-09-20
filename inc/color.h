#pragma once

#define cimg_display 0
#include "CImg.h"
#include "vec.h"
#include "interval.h"

using color = vec3;

void write_color(cimg_library::CImg<unsigned char> &image, unsigned int x, unsigned int y, color c)
{
    static const interval intensity(0.000, 0.999);
    image(x, y, 0) = intensity.clamp(c[0]) * 256;
    image(x, y, 1) = intensity.clamp(c[1]) * 256;
    image(x, y, 2) = intensity.clamp(c[2]) * 256;
}