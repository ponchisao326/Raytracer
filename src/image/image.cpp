#include "image/image.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <stdexcept>

namespace {

std::array<std::uint32_t, 256> make_crc_table() {
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t n = 0; n < 256; ++n) {
        std::uint32_t c = n;
        for (int k = 0; k < 8; ++k) {
            c = (c & 1) ? 0xEDB88320 ^ (c >> 1) : c >> 1;
        }
        table[n] = c;
    }
    return table;
}

void put_u32(std::vector<std::uint8_t>& out, std::uint32_t v) {
    out.push_back(v >> 24);
    out.push_back(v >> 16);
    out.push_back(v >> 8);
    out.push_back(v);
}

void put_chunk(std::vector<std::uint8_t>& out, const char* type, const std::vector<std::uint8_t>& data) {
    put_u32(out, data.size());
    std::size_t start = out.size();
    out.insert(out.end(), type, type + 4);
    out.insert(out.end(), data.begin(), data.end());
    put_u32(out, crc32(out.data() + start, out.size() - start));
}

std::vector<std::uint8_t> zlib_store(const std::vector<std::uint8_t>& raw) {
    std::vector<std::uint8_t> out{0x78, 0x01};
    std::size_t offset = 0;
    do {
        std::size_t len = std::min<std::size_t>(raw.size() - offset, 65535);
        bool last = offset + len == raw.size();
        out.push_back(last ? 1 : 0);
        out.push_back(len & 0xFF);
        out.push_back(len >> 8);
        out.push_back(~len & 0xFF);
        out.push_back((~len >> 8) & 0xFF);
        out.insert(out.end(), raw.begin() + offset, raw.begin() + offset + len);
        offset += len;
    } while (offset < raw.size());
    put_u32(out, adler32(raw.data(), raw.size()));
    return out;
}

bool write_file(const std::string& path, const std::vector<std::uint8_t>& bytes) {
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return file.good();
}

}

std::uint32_t crc32(const std::uint8_t* data, std::size_t size, std::uint32_t crc) {
    static const auto table = make_crc_table();
    crc = ~crc;
    for (std::size_t i = 0; i < size; ++i) {
        crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return ~crc;
}

std::uint32_t adler32(const std::uint8_t* data, std::size_t size) {
    std::uint32_t a = 1, b = 0;
    for (std::size_t i = 0; i < size; ++i) {
        a = (a + data[i]) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

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

bool Image::write_png(const std::string& path) const {
    std::vector<std::uint8_t> raw;
    raw.reserve(height_ * (1 + width_ * 3));
    for (int y = 0; y < height_; ++y) {
        raw.push_back(0);
        for (int x = 0; x < width_; ++x) {
            Pixel p = get(x, y);
            raw.push_back(p.r);
            raw.push_back(p.g);
            raw.push_back(p.b);
        }
    }

    std::vector<std::uint8_t> ihdr;
    put_u32(ihdr, width_);
    put_u32(ihdr, height_);
    ihdr.insert(ihdr.end(), {8, 2, 0, 0, 0});

    std::vector<std::uint8_t> png{0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    put_chunk(png, "IHDR", ihdr);
    put_chunk(png, "IDAT", zlib_store(raw));
    put_chunk(png, "IEND", {});
    return write_file(path, png);
}
