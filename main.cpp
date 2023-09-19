#include <iostream>

#include "CImg.h"
#include "vec.h"
#include "color.h"
#include "ray.h"
#include "sphere.h"

auto *sph1 = new sphere({0, 0, -2}, 0.5);

color ray_color(const ray &ray)
{
    hit_record rec;
    if (sph1->hit(ray, 0, 1e9, rec))
    {
        return color(rec.normal + vec3(1, 1, 1)) * 0.5;
    }
    auto dir = unit(ray.direction());
    double a = (dir.y() + 1) / 2;
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    using namespace cimg_library;
    // Image
    int image_width = 960;
    double aspect_ratio = 16.0 / 9.0;
    // Calculate image height
    int image_height = image_width / aspect_ratio;
    if (image_height < 1)
        image_height = 1;
    CImg<unsigned char> image(image_width, image_height, 1, 3);
    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
    auto camera_center = vec3(0, 0, 0);
    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    /*   x
    +---------->
    |..........
   y|..........
    |..........
    v
    */
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    for (int j = 0; j < image_height; ++j)
    {
        std::clog << "\rRemaining scanlines: " << (image_height - j) << " " << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
            ray ray(
                pixel_center,
                pixel_center - camera_center);
            auto pixel_color = ray_color(ray);
            write_color(image, i, j, pixel_color);
        }
    }
    std::clog << std::endl;
    image.save_png("test.png");
    return 0;
}