// math3d.h — minimal 4x4 matrix helpers for test task 01 (both tracks).
//
// Helper code, not part of the task: it saves you from writing matrix
// multiplication by hand. Matrices are column-major (m[column][row]), which
// matches simd_float4x4 in Metal and mat4 in GLSL (std140), so a Mat4 can be
// memcpy'd into a uniform buffer as is.
//
// One function is intentionally left for you: Perspective() — see TODO(projection).
// You may replace this header with simd/glm, but explain the choice in README.
#pragma once

#include <cmath>

namespace bhs {

struct Mat4 {
    float m[4][4];  // m[column][row]
};

inline Mat4 Identity() {
    Mat4 r{};
    for (int i = 0; i < 4; ++i) r.m[i][i] = 1.0f;
    return r;
}

// r = a * b (b is applied first).
inline Mat4 Multiply(const Mat4& a, const Mat4& b) {
    Mat4 r{};
    for (int c = 0; c < 4; ++c)
        for (int row = 0; row < 4; ++row) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k) s += a.m[k][row] * b.m[c][k];
            r.m[c][row] = s;
        }
    return r;
}

inline Mat4 Translation(float x, float y, float z) {
    Mat4 r = Identity();
    r.m[3][0] = x;
    r.m[3][1] = y;
    r.m[3][2] = z;
    return r;
}

// Rotation around a unit axis (x, y, z) by `radians`, right-handed.
inline Mat4 Rotation(float radians, float x, float y, float z) {
    const float c = std::cos(radians), s = std::sin(radians), t = 1.0f - c;
    Mat4 r = Identity();
    r.m[0][0] = t * x * x + c;     r.m[0][1] = t * x * y + s * z; r.m[0][2] = t * x * z - s * y;
    r.m[1][0] = t * x * y - s * z; r.m[1][1] = t * y * y + c;     r.m[1][2] = t * y * z + s * x;
    r.m[2][0] = t * x * z + s * y; r.m[2][1] = t * y * z - s * x; r.m[2][2] = t * z * z + c;
    return r;
}

// TODO(projection): perspective projection for a right-handed view space (camera looks
// along -Z) that maps depth to NDC z in [0, 1] — the convention of both Metal
// and Vulkan (OpenGL uses [-1, 1], so OpenGL tutorials do not fit as is).
//   fovyRadians — vertical field of view, aspect — width / height of the
//   drawable (swapchain image) in PIXELS, nearZ / farZ > 0.
// For the Vulkan track also remember that clip-space Y points down.
inline Mat4 Perspective(float fovyRadians, float aspect, float nearZ, float farZ) {
    (void)fovyRadians; (void)aspect; (void)nearZ; (void)farZ;
    return Identity();  // replace with your implementation
}

}  // namespace bhs
