#pragma once

#include "CImg.h"

#include "utils.h"
#include "hittable.h"
#include "material.h"
#include "indicators.h"

#include <thread>
#include <vector>

class camera
{
public:
    /* Public Camera Parameters Here */
    double aspect_ratio = 1.0;
    int image_width = 720;
    int image_height;        // will be calculated in initialize()
    int samples_per_row = 2; // stratified sampling
    int samples_per_subpixel = 1;
    int max_depth = 10;
    double vfov = 90;
    point3 lookat = point3(0, 0, -1);
    point3 lookfrom = point3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    double defocus_angle = 0;
    double focus_dist = 10;

    void render(cimg_library::CImg<unsigned char> &image, const hittable &world, int n_workers = 0) const
    {
        using namespace std;

        atomic_uint32_t progress;
        const int max_workers = thread::hardware_concurrency();
        if (n_workers == 0)
            n_workers = max_workers;
        clog << "Using " << n_workers << " workers" << endl;
        // Single Thread Rendering
        if (n_workers == 1)
        {
            for (int y = 0; y < image_height; ++y)
            {
                clog << "\rRemaining scanlines: " << (image_height - y) << " " << flush;
                for (int x = 0; x < image_width; ++x)
                {
                    auto pixel_color = color(0, 0, 0);
                    for (int j = 0; j < samples_per_row; j++)
                        for (int i = 0; i < samples_per_row; i++)
                        {
                            auto subpixel_color = color(0, 0, 0);
                            for (int k = 0; k < samples_per_subpixel; k++)
                            {
                                ray ray = get_ray(x, y, i, j);
                                subpixel_color += ray_color(ray, max_depth, world);
                            }
                            subpixel_color /= samples_per_subpixel;
                            pixel_color += subpixel_color;
                        }
                    pixel_color /= (samples_per_row * samples_per_row);
                    write_color(image, x, y, pixel_color);
                }
            }
            clog << endl;
        }
        // Multiple Threads Rendering
        else
        {
            using namespace indicators;

            // Hide cursor
            show_console_cursor(false);

            indicators::ProgressBar bar{
                option::BarWidth{50},
                option::Start{" ["},
                option::Fill{"¨€"},
                option::Lead{"¨€"},
                option::Remainder{"-"},
                option::End{"]"},
                option::ShowPercentage{true},
                option::PrefixText{"Rendering"},
                option::ForegroundColor{Color::yellow},
                option::ShowElapsedTime{true},
                option::ShowRemainingTime{true},
                option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
            };

            auto worker = [&](int id)
            {
                int y = 0, x = id, n_pixels = image_height * image_width;
                while (y < image_height)
                {
                    auto pixel_color = color(0, 0, 0);
                    for (int j = 0; j < samples_per_row; j++)
                        for (int i = 0; i < samples_per_row; i++)
                        {
                            auto subpixel_color = color(0, 0, 0);
                            for (int k = 0; k < samples_per_subpixel; k++)
                            {
                                ray ray = get_ray(x, y, i, j);
                                subpixel_color += ray_color(ray, max_depth, world);
                            }
                            subpixel_color /= samples_per_subpixel;
                            pixel_color += subpixel_color;
                        }
                    pixel_color /= (samples_per_row * samples_per_row);
                    write_color(image, x, y, pixel_color);
                    progress++;
                    if (progress * 100 % n_pixels == 0)
                        bar.tick();
                    x += n_workers;
                    if (x >= image_width)
                    {
                        x -= image_width;
                        y++;
                    }
                }
            };

            progress = 0;
            vector<thread> threads;
            for (int i = 0; i < n_workers; i++)
                threads.push_back(thread(worker, i));
            for (auto &t : threads)
                t.join();

            // Show cursor
            show_console_cursor(true);
        }
    }

    void initialize()
    {
        image_height = image_width / aspect_ratio;
        if (image_height < 1)
            image_height = 1;
        // Camera
        center = lookfrom;
        auto theta = deg_to_rad(vfov);
        auto h = tan(theta / 2);
        viewport_height = 2 * h * focus_dist;
        viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        w = unit(lookfrom - lookat);
        u = unit(cross(vup, w));
        v = cross(w, u);
        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        /*   x
        +---------->
        |..........
       y|..........
        |..........
        v
        */
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * (-v);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left;

        // Calculate the camera depth of field disk basic vectors
        auto defocus_radius = focus_dist * tan(deg_to_rad(defocus_angle) / 2);
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

private:
    /* Private Camera Variables Here */
    double viewport_height;
    double viewport_width;
    point3 pixel00_loc; // Location of pixel 0, 0
    vec3 pixel_delta_u; // Offset to pixel to the right
    vec3 pixel_delta_v; // Offset to pixel below
    point3 center;
    vec3 u, v, w; // right, up, opposite view direction
    vec3 defocus_disk_u, defocus_disk_v;

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
                             (j + random_double()) * pixel_delta_v) /
                                samples_per_row;

        auto ray_origin = (defocus_angle <= 0.0) ? center : defocus_disk_sample();
        auto ray_direction = jittered_pos - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    point3 defocus_disk_sample() const
    {
        auto p = random_in_unit_disk();
        return center + defocus_disk_u * p.x() + defocus_disk_v * p.y();
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