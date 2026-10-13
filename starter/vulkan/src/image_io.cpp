// image_io.cpp — see image_io.h.
#include "image_io.h"

#include <cstring>
#include <stdexcept>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"  // third_party/stb, v2.30
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace bhs {

ImageRGBA8 LoadRGBA8(const std::string& path) {
    int w = 0, h = 0, channelsInFile = 0;
    stbi_uc* data = stbi_load(path.c_str(), &w, &h, &channelsInFile, STBI_rgb_alpha);
    if (!data) {
        throw std::runtime_error("cannot load image '" + path + "': " + stbi_failure_reason());
    }
    ImageRGBA8 image;
    image.width = static_cast<uint32_t>(w);
    image.height = static_cast<uint32_t>(h);
    image.pixels.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
    std::memcpy(image.pixels.data(), data, image.pixels.size());
    stbi_image_free(data);
    return image;
}

}  // namespace bhs
