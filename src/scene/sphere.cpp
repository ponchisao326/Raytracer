#include "scene/sphere.h"

#include <cmath>

Intersection intersect(const Ray& ray, const Sphere& s) {
    Vec3 oc = ray.origin - s.center;

    Intersection hit;
    hit.a = dot(ray.direction, ray.direction);
    hit.b = 2.0 * dot(ray.direction, oc);
    hit.c = dot(oc, oc) - s.radius * s.radius;
    hit.discriminant = hit.b * hit.b - 4 * hit.a * hit.c;
    if (hit.discriminant < 0) {
        return hit;
    }

    double sq = std::sqrt(hit.discriminant);
    double t1 = (-hit.b - sq) / (2 * hit.a);
    double t2 = (-hit.b + sq) / (2 * hit.a);
    if (t1 > 0) {
        hit.t = t1;
    } else if (t2 > 0) {
        hit.t = t2;
    }
    return hit;
}

Vec3 normal_at(const Sphere& s, const Vec3& p) {
    return (p - s.center) / s.radius;
}
