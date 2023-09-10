#ifndef TEXTURE_H
#define TEXTURE_H

#include "RTWeekend.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class texture {
    public:
        virtual ~texture() = default;

        virtual Color value(double u, double v, const Point3& p) const = 0;
};

class solid_color : public texture {
    public:
        solid_color(Color c) : color_value(c) {}

        solid_color(double red, double green, double blue) : solid_color(Color(red, green, blue)) {}

        Color value(double u, double v, const Point3& p) const override {
            return color_value;
        }
    
    private:
        Color color_value;
};

class checker_texture : public texture {
    public:
        checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd)
            : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}
        
        checker_texture(double _scale, Color c1, Color c2)
            : inv_scale(1.0 / _scale),
              even(make_shared<solid_color>(c1)),
              odd(make_shared<solid_color>(c2))
        {}

        Color value(double u, double v, const Point3& p) const override {
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

class image_texture : public texture {
    public:
        image_texture(const char* filename) : image(filename) {}

        Color value(double u, double v, const Point3& p) const override {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (image.height() <= 0) return Color(0, 1, 1);

            // Clamp input texture coordinates to [0,1] x [1,0]
            u = interval(0, 1).clamp(u);
            v = 1.0 - interval(0, 1).clamp(v); // Filp V to image coordinates

            auto i = static_cast<int>(u * image.width());
            auto j = static_cast<int>(v * image.height());
            auto pixel = image.pixel_data(i, j);

            auto color_scale = 1.0 / 255.0;
            return Color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
        }

    private:
        rtw_image image;
};

class noise_texture : public texture {
    public:
        noise_texture() {}

        noise_texture(double sc) : scale(sc) {}

        Color value(double u, double v, const Point3& p) const override {
            auto s = scale * p;
            return Color(1,1,1) * 0.5 * (1 + sin(s.z() + 10*noise.turb(s)));
        }

    private:
        perlin noise;
        double scale;
};

#endif