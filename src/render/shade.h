#pragma once

#include "render/color.h"
#include "render/ray.h"
#include "scene/scene.h"

constexpr Color sphere_color{1.0, 0.55, 0.1};

Color sky(const Ray& ray);
Color shade_flat(const Ray& ray, const Scene& scene);
