#pragma once

#include "math/vec3.h"
#include "render/ray.h"

#include <optional>

struct Sphere {
    Vec3 center;
    double radius = 1;
};

struct Intersection {
    double a = 0, b = 0, c = 0;
    double discriminant = 0;
    std::optional<double> t;
};

Intersection intersect(const Ray& ray, const Sphere& s);
Vec3 normal_at(const Sphere& s, const Vec3& p);
