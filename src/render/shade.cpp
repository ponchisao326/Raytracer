#include "render/shade.h"

#include <algorithm>

Color sky(const Ray& ray) {
    double t = 0.5 * (normalized(ray.direction).y + 1.0);
    return lerp({0.04, 0.07, 0.14}, {0.16, 0.24, 0.42}, t);
}

double diffuse(const Scene& scene, const Vec3& p) {
    return std::max(0.0, dot(normal_at(scene.sphere, p), scene.light_dir));
}

Color shade_flat(const Ray& ray, const Scene& scene) {
    if (intersect(ray, scene.sphere).t) {
        return sphere_color;
    }
    return sky(ray);
}

Color shade(const Ray& ray, const Scene& scene) {
    auto hit = intersect(ray, scene.sphere);
    if (!hit.t) {
        return sky(ray);
    }
    return diffuse(scene, ray.at(*hit.t)) * sphere_color;
}
