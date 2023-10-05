#pragma once

#include "utils.h"
#include "CImg.h"

class texture
{
public:
    virtual ~texture() = default;
    virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture
{
private:
    color color_value;

public:
    solid_color(color c) : color_value(c) {}
    solid_color(double r, double g, double b) : color_value(r, g, b) {}
    color value(double u, double v, const point3 &p) const override
    {
        return color_value;
    }
};

class checker_texture : public texture
{
public:
    checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

    checker_texture(double _scale, color c1, color c2)
        : inv_scale(1.0 / _scale),
          even(make_shared<solid_color>(c1)),
          odd(make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point3 &p) const override
    {
        auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
        auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
        auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class image_texture: public texture
{
private:
    cimg_library::CImg<unsigned char> image;
    

public:
    image_texture(const std::string &filename)
    {
        if (load(filename)) return;
        if (load("images/" + filename)) return;
        if (load("../images/" + filename)) return;
        if (load("../../images/" + filename)) return;
        if (load("../../../images/" + filename)) return;
        if (load("../../../../images/" + filename)) return;
        std::cerr << "texture image "<< filename << "not found" << std::endl;
    }

    // load an image from path
    bool load(const std::string &filepath)
    {
        try
        {
            image = std::move(cimg_library::CImg<unsigned char>(filepath.c_str()));
        }
        catch(const cimg_library::CImgIOException &e)
        {
            // std::cerr << e.what() << '\n';
        }
        return !image.is_empty();
    }

    // use bilinear interpolation to sample
    color value(double u, double v, const point3 &p) const
    {
        if (image.is_empty())
            return color(0, 1, 1);

        u = image.width() * u;
        v = (1 - v) * image.height();
        auto pixel = color(image._linear_atXY(u, v, 0, 0), 
                           image._linear_atXY(u, v, 0, 1),
                           image._linear_atXY(u, v, 0, 2));

        auto color_scale = 1.0 / 255;
        return pixel * color_scale;
    }
};