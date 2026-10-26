#pragma once

#include "image/image.h"

struct Color {
    double r = 0;
    double g = 0;
    double b = 0;
};

Color lerp(const Color& a, const Color& b, double t);
Color operator*(double k, const Color& c);
Pixel to_pixel(const Color& c);
