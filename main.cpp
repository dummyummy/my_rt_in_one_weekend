#include <iostream>

#include "CImg.h"

#include "utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

int main()
{
    using namespace cimg_library;
    // World
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.5);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.center = point3(0, 0, 1);
    cam.image_width = 400;
    cam.focal_length = 1;
    cam.samples_per_row = 8;
    cam.max_depth = 50;
    cam.initialize();
    CImg<unsigned char> image(cam.image_width, cam.image_height, 1, 3);
    cam.render(image, world);
    image.save_png("test.png");
    return 0;
}