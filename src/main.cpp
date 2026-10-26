#include "image/image.h"
#include "render/camera.h"
#include "render/shade.h"
#include "scene/scene.h"

#include <filesystem>
#include <iostream>

int main() {
    const int width = 640, height = 360;
    Camera cam(width, height);
    Scene scene{{{0, 0, -3}, 1}};

    Image image(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image.set(x, y, to_pixel(shade_flat(cam.get_ray(x, y), scene)));
        }
    }

    std::filesystem::create_directories("out");
    if (!image.write_png("out/image.png") || !image.write_ppm("out/image.ppm")) {
        std::cerr << "error saving out/image\n";
        return 1;
    }
    return 0;
}
