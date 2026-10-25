#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Pixel {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
};

class Image {
public:
    Image(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }

    Pixel get(int x, int y) const;
    void set(int x, int y, Pixel p);

    bool write_ppm(const std::string& path) const;

private:
    std::size_t index(int x, int y) const;

    int width_;
    int height_;
    std::vector<Pixel> pixels_;
};
