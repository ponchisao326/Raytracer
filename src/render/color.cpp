#include "render/color.h"

#include <algorithm>
#include <cmath>

static std::uint8_t to_byte(double v) {
    return std::lround(std::clamp(v, 0.0, 1.0) * 255);
}

Color lerp(const Color& a, const Color& b, double t) {
    return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t};
}

Color operator*(double k, const Color& c) {
    return {k * c.r, k * c.g, k * c.b};
}

Pixel to_pixel(const Color& c) {
    return {to_byte(c.r), to_byte(c.g), to_byte(c.b)};
}
