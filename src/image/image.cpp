#include "image/image.h"

#include <fstream>
#include <stdexcept>

Image::Image(int width, int height) : width_(width), height_(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("invalid image size");
    }
    pixels_.resize(width * height);
}

std::size_t Image::index(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        throw std::out_of_range("pixel out of bounds");
    }
    return y * width_ + x;
}

Pixel Image::get(int x, int y) const {
    return pixels_[index(x, y)];
}

void Image::set(int x, int y, Pixel p) {
    pixels_[index(x, y)] = p;
}

bool Image::write_ppm(const std::string& path) const {
    std::ofstream file(path);
    file << "P3\n" << width_ << ' ' << height_ << "\n255\n";
    for (const Pixel& p : pixels_) {
        file << int(p.r) << ' ' << int(p.g) << ' ' << int(p.b) << '\n';
    }
    return file.good();
}
