#include "image/image.h"
#include "render/camera.h"
#include "render/color.h"
#include "render/shade.h"
#include "scene/scene.h"

#include <charconv>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace {

using Shader = Color (*)(const Ray&, const Scene&);

struct Options {
    int width = 640;
    int height = 360;
    bool flat = false;
    int frame_rows = 0;
    bool trace = false;
    int trace_x = 0;
    int trace_y = 0;
};

bool parse_int(std::string_view s, int& out) {
    auto [end, err] = std::from_chars(s.data(), s.data() + s.size(), out);
    return err == std::errc() && end == s.data() + s.size();
}

bool parse_size(std::string_view s, int& w, int& h) {
    auto x = s.find('x');
    if (x == std::string_view::npos) {
        return false;
    }
    return parse_int(s.substr(0, x), w) && parse_int(s.substr(x + 1), h) && w > 0 && h > 0;
}

std::optional<Options> parse_args(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--flat") {
            opt.flat = true;
        } else if (arg == "--size" && i + 1 < argc) {
            if (!parse_size(argv[++i], opt.width, opt.height)) {
                return std::nullopt;
            }
        } else if (arg == "--frames" && i + 1 < argc) {
            if (!parse_int(argv[++i], opt.frame_rows) || opt.frame_rows <= 0) {
                return std::nullopt;
            }
        } else if (arg == "--trace" && i + 2 < argc) {
            opt.trace = true;
            if (!parse_int(argv[++i], opt.trace_x) || !parse_int(argv[++i], opt.trace_y)) {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }
    if (opt.trace) {
        bool inside = opt.trace_x >= 0 && opt.trace_y >= 0 && opt.trace_x < opt.width && opt.trace_y < opt.height;
        if (!inside || opt.frame_rows > 0) {
            return std::nullopt;
        }
    }
    return opt;
}

void print(const char* name, const Vec3& v) {
    std::printf("%s: (%.5f, %.5f, %.5f)\n", name, v.x, v.y, v.z);
}

int trace(const Options& opt, const Camera& cam, const Scene& scene, Shader shader) {
    Ray ray = cam.get_ray(opt.trace_x, opt.trace_y);
    Intersection hit = intersect(ray, scene.sphere);
    Color color = shader(ray, scene);
    Pixel px = to_pixel(color);

    std::printf("pixel %d,%d (%dx%d)\n", opt.trace_x, opt.trace_y, opt.width, opt.height);
    print("origin", ray.origin);
    print("dir", ray.direction);
    print("center", scene.sphere.center);
    std::printf("radius: %.5f\n", scene.sphere.radius);
    std::printf("a=%.5f b=%.5f c=%.5f\n", hit.a, hit.b, hit.c);
    std::printf("disc: %.5f\n", hit.discriminant);
    if (hit.t) {
        Vec3 p = ray.at(*hit.t);
        std::printf("t: %.5f\n", *hit.t);
        print("hit", p);
        if (!opt.flat) {
            print("normal", normal_at(scene.sphere, p));
            print("light", scene.light_dir);
            std::printf("diffuse: %.5f\n", diffuse(scene, p));
        }
    } else {
        std::printf("miss\n");
    }
    std::printf("color: (%.3f, %.3f, %.3f) -> %d %d %d\n", color.r, color.g, color.b, px.r, px.g, px.b);
    return 0;
}

int render(const Options& opt, const Camera& cam, const Scene& scene, Shader shader) {
    Image image(opt.width, opt.height);
    for (int y = 0; y < opt.height; ++y) {
        for (int x = 0; x < opt.width; ++x) {
            image.set(x, y, to_pixel(shader(cam.get_ray(x, y), scene)));
        }
        if (opt.frame_rows > 0 && ((y + 1) % opt.frame_rows == 0 || y + 1 == opt.height)) {
            if (!image.write_rgb(std::cout)) {
                std::cerr << "error writing frame\n";
                return 1;
            }
        }
    }

    std::string name = opt.flat ? "out/flat" : "out/image";
    std::filesystem::create_directories("out");
    if (!image.write_png(name + ".png") || !image.write_ppm(name + ".ppm")) {
        std::cerr << "error saving " << name << "\n";
        return 1;
    }
    std::cerr << "saved " << name << ".png\n";
    return 0;
}

}

int main(int argc, char** argv) {
    auto opt = parse_args(argc, argv);
    if (!opt) {
        std::cerr << "usage: raytracer [--size WxH] [--flat] [--frames N] [--trace X Y]\n";
        return 2;
    }

    Camera cam(opt->width, opt->height);
    Scene scene{{{0, 0, -3}, 1}, normalized({-1, 1, 1})};
    Shader shader = opt->flat ? shade_flat : shade;

    if (opt->trace) {
        return trace(*opt, cam, scene, shader);
    }
    return render(*opt, cam, scene, shader);
}
