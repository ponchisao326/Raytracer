#pragma once

#include "render/color.h"
#include "render/ray.h"
#include "scene/scene.h"

constexpr Color sphere_color{1.0, 0.55, 0.1};

Color sky(const Ray& ray);
double diffuse(const Scene& scene, const Vec3& p);
Color shade_flat(const Ray& ray, const Scene& scene);
Color shade(const Ray& ray, const Scene& scene);
