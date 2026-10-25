#include "image/image.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(bool ok, const char* what) {
    if (!ok) {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", what);
    }
}

std::vector<std::uint8_t> read_bytes(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

std::uint32_t read_u32(const std::vector<std::uint8_t>& bytes, std::size_t at) {
    return (static_cast<std::uint32_t>(bytes[at]) << 24) | (static_cast<std::uint32_t>(bytes[at + 1]) << 16) |
           (static_cast<std::uint32_t>(bytes[at + 2]) << 8) | static_cast<std::uint32_t>(bytes[at + 3]);
}

std::uint32_t checksum_of(const std::string& text, bool adler) {
    const auto* data = reinterpret_cast<const std::uint8_t*>(text.data());
    return adler ? adler32(data, text.size()) : crc32(data, text.size());
}

}

int main() {
    check(checksum_of("IEND", false) == 0xAE426082u, "crc32 of IEND");
    check(checksum_of("Wikipedia", true) == 0x11E60398u, "adler32 of Wikipedia");

    Image image(2, 1);
    image.set(0, 0, {255, 0, 0});
    image.set(1, 0, {0, 128, 255});
    check(image.get(1, 0).g == 128, "get returns what set stored");

    bool threw = false;
    try {
        image.get(2, 0);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    check(threw, "a pixel outside the image throws");

    const auto dir = std::filesystem::temp_directory_path() / "raytracer_image_tests";
    std::filesystem::create_directories(dir);

    check(image.write_ppm((dir / "two.ppm").string()), "write_ppm succeeds");
    std::ifstream ppm(dir / "two.ppm");
    std::stringstream text;
    text << ppm.rdbuf();
    check(text.str() == "P3\n2 1\n255\n255 0 0\n0 128 255\n", "the ppm is plain text, one pixel per line");

    check(image.write_png((dir / "two.png").string()), "write_png succeeds");
    const auto png = read_bytes(dir / "two.png");
    const std::vector<std::uint8_t> signature{0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    check(png.size() == 75, "png size: signature, IHDR, one stored block, IEND");
    check(png.size() >= 8 && std::equal(signature.begin(), signature.end(), png.begin()), "png signature");
    if (png.size() == 75) {
        check(read_u32(png, 16) == 2 && read_u32(png, 20) == 1, "IHDR holds the width and the height");
        check(png[48] == 0 && png[49] == 255 && png[53] == 128 && png[54] == 255,
              "the pixels are stored unfiltered and uncompressed");
        check(read_u32(png, 59) == crc32(png.data() + 37, 22), "the IDAT checksum covers its type and data");
    }

    std::filesystem::remove_all(dir);

    if (failures == 0) {
        std::printf("all image tests pass\n");
    }
    return failures == 0 ? 0 : 1;
}
