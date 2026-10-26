#pragma once

#include <cmath>

struct Vec3 {
    double x = 0, y = 0, z = 0;
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 operator-(const Vec3& a, const Vec3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 operator-(const Vec3& v) {
    return {-v.x, -v.y, -v.z};
}

inline Vec3 operator*(double t, const Vec3& v) {
    return {t * v.x, t * v.y, t * v.z};
}

inline Vec3 operator*(const Vec3& v, double t) {
    return t * v;
}

inline Vec3 operator/(const Vec3& v, double t) {
    return {v.x / t, v.y / t, v.z / t};
}

inline double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalized(const Vec3& v) {
    return v / length(v);
}
