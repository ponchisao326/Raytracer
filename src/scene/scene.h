#pragma once

#include "math/vec3.h"
#include "scene/sphere.h"

struct Scene {
    Sphere sphere;
    Vec3 light_dir;
};
