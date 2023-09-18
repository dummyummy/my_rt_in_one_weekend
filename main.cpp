#include <iostream>

#include "vec.hpp"
#include "CImg.h"
#include "ray.hpp"

int main()
{
    // Image
    int image_width = 256;
    int image_height = 256;
    using namespace cimg_library;
    CImg<unsigned char> image(image_width, image_height, 1, 3);
    // Render

    for (int j = 0; j < image_height; ++j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_height - 1);
            auto b = 0;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            image(i, j, 0) = ir;
            image(i, j, 1) = ig;
            image(i, j, 2) = ib;
        }
    }
    image.save_png("test.png");
    return 0;
}