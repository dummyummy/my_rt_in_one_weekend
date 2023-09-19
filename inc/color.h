#pragma once

#define cimg_display 0
#include "CImg.h"
#include "vec.h"

using color = vec3;

void write_color(cimg_library::CImg<unsigned char> &image, unsigned int x, unsigned int y, color c)
{
    c *= 255.99;
    image(x, y, 0) = c[0];
    image(x, y, 1) = c[1];
    image(x, y, 2) = c[2];
}