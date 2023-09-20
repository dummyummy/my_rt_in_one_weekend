#include <iostream>

#include "CImg.h"

#include "utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

int main()
{
    using namespace cimg_library;
    // World
    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.center = point3(0, 0, 0);
    cam.image_width = 400;
    cam.focal_length = 1;
    cam.samples_per_row = 4;
    cam.initialize();
    CImg<unsigned char> image(cam.image_width, cam.image_height, 1, 3);
    cam.render(image, world);
    image.save_png("test.png");
    return 0;
}