#include "image/image.h"
#include "render/camera.h"
#include "render/shade.h"

#include <filesystem>
#include <iostream>

int main() {
    const int width = 640, height = 360;
    Camera cam(width, height);
    Image image(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image.set(x, y, to_pixel(sky(cam.get_ray(x, y))));
        }
    }

    std::filesystem::create_directories("out");
    if (!image.write_png("out/image.png") || !image.write_ppm("out/image.ppm")) {
        std::cerr << "error saving out/image\n";
        return 1;
    }
    return 0;
}
