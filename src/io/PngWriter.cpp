#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ir/io/PngWriter.hpp>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace ir::io {
namespace {

[[nodiscard]] std::uint8_t toByteClamp01(double value) {
    const double clamped = std::min(1.0, std::max(0.0, value));
    const double scaled = std::lround(clamped * 255.0);
    return static_cast<std::uint8_t>(std::min(255.0, std::max(0.0, scaled)));
}

[[nodiscard]] std::vector<std::uint8_t> toRasterU8(const Image2D& image, ScalingMode scaling) {
    const int width = image.width();
    const int height = image.height();
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(width * height));

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            const double value = image.at(i, j);
            std::uint8_t sample = 0;
            switch (scaling) {
            case ScalingMode::Clamp01:
                sample = toByteClamp01(value);
                break;
            }
            bytes[static_cast<std::size_t>(j * width + i)] = sample;
        }
    }
    return bytes;
}

} // namespace

bool PngWriter::write(const Image2D& image, const std::string& path,
                      const ExportOptions& options) const {
    if (path.empty()) {
        return false;
    }

    const int width = image.width();
    const int height = image.height();
    if (width < 1 || height < 1) {
        return false;
    }

    const std::vector<std::uint8_t> bytes = toRasterU8(image, options.scaling);
    const int stride = width; // 1 byte per pixel, packed rows

    const int ok = stbi_write_png(path.c_str(), width, height, 1, bytes.data(), stride);
    return ok != 0;
}

} // namespace ir::io
