#pragma once

#include "render/ray.h"

class Camera {
public:
    Camera(int width, int height);

    Ray get_ray(int x, int y) const;

private:
    Vec3 origin_{0, 0, 0};
    int width_;
    int height_;
    double viewport_w_;
    double viewport_h_ = 2.0;
};
