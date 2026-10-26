#include "render/camera.h"

Camera::Camera(int width, int height) : width_(width), height_(height) {
    viewport_w_ = viewport_h_ * width / height;
}

Ray Camera::get_ray(int x, int y) const {
    double u = (x + 0.5) / width_;
    double v = (y + 0.5) / height_;
    Vec3 target{(u - 0.5) * viewport_w_, (0.5 - v) * viewport_h_, -1};
    return {origin_, normalized(target - origin_)};
}
