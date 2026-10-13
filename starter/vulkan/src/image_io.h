// image_io.h — PNG decoding for the "no Mac" track.
//
// Helper code, not part of the task: it plays the role MTKTextureLoader's
// file decoding plays on the Mac track. It only reads the file into CPU
// memory. Getting the pixels to the GPU is your part (see main.cpp).
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace bhs {

struct ImageRGBA8 {
    uint32_t width = 0;
    uint32_t height = 0;
    // width * height * 4 bytes, rows top to bottom, R G B A per pixel.
    // Values are stored as in the file (sRGB-encoded for checker.png).
    std::vector<uint8_t> pixels;
};

// Decodes a PNG (or any format stb_image supports) into tightly packed
// RGBA8. Throws std::runtime_error with the path and the reason on failure.
ImageRGBA8 LoadRGBA8(const std::string& path);

}  // namespace bhs
