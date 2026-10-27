#include "math/vec3.h"
#include "render/camera.h"
#include "render/color.h"
#include "render/ray.h"
#include "render/shade.h"
#include "scene/scene.h"
#include "scene/sphere.h"

#include <cmath>
#include <cstdio>

namespace {

int failures = 0;

void check(bool ok, const char* what) {
    if (!ok) {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", what);
    }
}

bool near(double a, double b) {
    return std::fabs(a - b) < 1e-9;
}

bool near(const Vec3& a, const Vec3& b) {
    return near(a.x, b.x) && near(a.y, b.y) && near(a.z, b.z);
}

const Sphere ball{{0.0, 0.0, -3.0}, 1.0};
const Scene scene{ball, normalized(Vec3{-1.0, 1.0, 1.0})};

void vectors() {
    const Vec3 a{1.0, 2.0, 3.0};
    const Vec3 b{4.0, -5.0, 6.0};
    check(near(a + b, {5.0, -3.0, 9.0}), "a + b");
    check(near(a - b, {-3.0, 7.0, -3.0}), "a - b");
    check(near(-a, {-1.0, -2.0, -3.0}), "-a");
    check(near(2.0 * a, a * 2.0) && near(2.0 * a, {2.0, 4.0, 6.0}), "t * a and a * t");
    check(near(dot(a, b), 12.0), "dot");
    check(near(length(Vec3{3.0, 4.0, 0.0}), 5.0), "length");
    check(near(length(normalized(b)), 1.0), "normalized has length 1");
}

void rays() {
    const Ray ray{{1.0, 1.0, 1.0}, {0.0, 0.0, -1.0}};
    check(near(ray.at(0.0), ray.origin), "at(0) is the origin");
    check(near(ray.at(2.5), {1.0, 1.0, -1.5}), "at(t) moves t along the direction");
}

void spheres() {
    const Intersection straight = intersect({{0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}}, ball);
    check(straight.t.has_value() && near(*straight.t, 2.0), "a ray straight at the ball hits it at t = 2");
    check(near(straight.discriminant, 4.0), "and its discriminant is 4");

    const Intersection up = intersect({{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}}, ball);
    check(up.discriminant < 0 && !up.t.has_value(), "a ray going up misses");

    const Intersection graze = intersect({{1.0, 0.0, 0.0}, {0.0, 0.0, -1.0}}, ball);
    check(near(graze.discriminant, 0.0) && graze.t.has_value() && near(*graze.t, 3.0),
          "a ray that grazes the ball has discriminant 0 and touches it once");

    const Intersection inside = intersect({ball.center, {0.0, 1.0, 0.0}}, ball);
    check(inside.t.has_value() && near(*inside.t, 1.0), "from inside, the ray leaves through the far side");

    const Intersection behind = intersect({{0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}}, ball);
    check(behind.discriminant > 0 && !behind.t.has_value(), "a ball behind the ray is not hit");
}

void cameras() {
    const Camera small(3, 3);
    check(near(small.get_ray(1, 1).direction, {0.0, 0.0, -1.0}), "the middle pixel looks straight ahead");
    const Ray corner = small.get_ray(0, 0);
    check(corner.direction.x < 0 && corner.direction.y > 0, "the top left pixel looks up and to the left");
    check(near(length(corner.direction), 1.0), "ray directions have length 1");
    check(near(corner.origin, {0.0, 0.0, 0.0}), "every ray starts at the eye");
}

void colours() {
    const Pixel pixel = to_pixel({1.0, 0.5, -1.0});
    check(pixel.r == 255 && pixel.g == 128 && pixel.b == 0, "to_pixel rounds and clamps");

    const Ray at_ball{{0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}};
    const Color flat = shade_flat(at_ball, scene);
    check(near(flat.r, sphere_color.r) && near(flat.g, sphere_color.g) && near(flat.b, sphere_color.b),
          "without light, a ray that hits takes the ball's colour");
    const Color lit = shade(at_ball, scene);
    const double third = 1.0 / std::sqrt(3.0);
    check(near(lit.r, third * sphere_color.r) && near(lit.g, third * sphere_color.g) &&
              near(lit.b, third * sphere_color.b),
          "with light, the front of the ball gets the colour times its brightness");

    const Ray away{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};
    const Color expected = sky(away);
    const Color miss = shade(away, scene);
    const Color flat_miss = shade_flat(away, scene);
    check(near(miss.r, expected.r) && near(miss.g, expected.g) && near(miss.b, expected.b) &&
              near(flat_miss.b, expected.b),
          "a ray that misses takes the sky's colour");
}

void light() {
    check(near(normal_at(ball, {0.0, 0.0, -2.0}), {0.0, 0.0, 1.0}), "the normal points out of the ball");
    check(near(diffuse(scene, ball.center + scene.light_dir), 1.0), "facing the light: full brightness");
    check(near(diffuse(scene, ball.center - scene.light_dir), 0.0), "facing away: no light at all");
    check(near(diffuse(scene, ball.center + normalized(Vec3{1.0, 1.0, 0.0})), 0.0), "side on to the light: no light");
}

}

int main() {
    vectors();
    rays();
    spheres();
    cameras();
    colours();
    light();

    if (failures == 0) {
        std::printf("all render tests pass\n");
    }
    return failures == 0 ? 0 : 1;
}
