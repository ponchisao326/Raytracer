#include "image/image.h"

#include <filesystem>
#include <iostream>

int main() {
    Image image(640, 360);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            std::uint8_t r = x * 255 / image.width();
            std::uint8_t g = y * 255 / image.height();
            image.set(x, y, {r, g, 64});
        }
    }

    std::filesystem::create_directories("out");
    if (!image.write_png("out/image.png") || !image.write_ppm("out/image.ppm")) {
        std::cerr << "error saving out/image\n";
        return 1;
    }
    return 0;
}
