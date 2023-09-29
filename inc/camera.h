#pragma once

#include "CImg.h"

#include "utils.h"
#include "hittable.h"
#include "material.h"

class camera
{
public:
    /* Public Camera Parameters Here */
    double aspect_ratio = 1.0;
    int image_width = 720;
    int image_height; // will be calculated in initialize()
    point3 center = point3(0, 0, 0);
    double focal_length = 1;
    int samples_per_row = 2; // stratified sampling
    int max_depth = 10;

    void render(cimg_library::CImg<unsigned char> &image, const hittable &world) const
    {
        // Render
        for (int y = 0; y < image_height; ++y)
        {
            std::clog << "\rRemaining scanlines: " << (image_height - y) << " " << std::flush;
            for (int x = 0; x < image_width; ++x)
            {
                auto pixel_color = color(0, 0, 0);
                for (int j = 0; j < samples_per_row; j++)
                    for (int i = 0; i < samples_per_row; i++)
                    {
                        ray ray = get_ray(x, y, i, j);
                        pixel_color += ray_color(ray, max_depth, world);
                    }
                pixel_color /= (samples_per_row * samples_per_row);
                write_color(image, x, y, pixel_color);
            }
        }
        std::clog << std::endl;
    }



    void initialize()
    {
        image_height = image_width / aspect_ratio;
        if (image_height < 1)
            image_height = 1;
        // Camera
        viewport_height = 2.0;
        viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
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
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left;
    }

private:
    /* Private Camera Variables Here */
    double viewport_height;
    double viewport_width;
    point3 pixel00_loc; // Location of pixel 0, 0
    vec3 pixel_delta_u; // Offset to pixel to the right
    vec3 pixel_delta_v; // Offset to pixel below

    /**
     * @brief get jittered samples using stratified sampling
     * @param i x position of sub pixel
     * @param j y position of sub pixel
     */
    ray get_ray(int x, int y, int i, int j) const
    {
        auto pixel_start = pixel00_loc + x * pixel_delta_u + y * pixel_delta_v; // upper left corner
        auto jittered_pos = pixel_start + 
                            ((i + random_double()) * pixel_delta_u + 
                            (j + random_double()) * pixel_delta_v) / samples_per_row;

        auto ray_origin = center;
        auto ray_direction = jittered_pos - ray_origin;

        return ray(ray_origin, ray_direction);
    }


    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        if (depth <= 0)
            return color(0, 0, 0);
        hit_record rec;
        if (world.hit(r, interval(0.001, inf), rec))
        {
            ray scattered;
            color attenuation;
            if (rec.mat->scattered(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            else
                return color(0, 0, 0);
        }
        auto dir = unit(r.direction());
        double a = (dir.y() + 1) * 0.5;
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};