#pragma once

#include <ir/core/Image2D.hpp>
#include <ir/io/ExportOptions.hpp>
#include <string>

namespace ir::io {

/**
 * Writes an Image2D to an 8-bit grayscale PNG file.
 *
 * Uses the shared export scaling policy (default Clamp01). Geometry metadata
 * (spacing, origin, direction) is not embedded in the PNG; the file is for
 * display and debugging. Core types remain free of I/O dependencies.
 */
class PngWriter {
public:
    /**
     * Write @p image to @p path as a grayscale PNG.
     * @param image Source image (row-major doubles).
     * @param path Output filesystem path (must be non-empty and writable).
     * @param options Scaling and future export options.
     * @return True on success; false if the path is invalid or encoding/write fails.
     */
    [[nodiscard]] bool write(const Image2D& image, const std::string& path,
                             const ExportOptions& options = ExportOptions{}) const;
};

/**
 * Convenience wrapper around PngWriter with default ExportOptions.
 * @param image Source image.
 * @param path Output path.
 * @return True on success.
 */
[[nodiscard]] inline bool writePng(const Image2D& image, const std::string& path) {
    return PngWriter{}.write(image, path);
}

} // namespace ir::io
