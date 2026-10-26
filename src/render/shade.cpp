#include "render/shade.h"

Color sky(const Ray& ray) {
    double t = 0.5 * (normalized(ray.direction).y + 1.0);
    return lerp({0.04, 0.07, 0.14}, {0.16, 0.24, 0.42}, t);
}
