#pragma once

#include <cassert>
#include <cstddef>
#include <ir/core/ImageGeometry2D.hpp>
#include <utility>
#include <vector>

namespace ir {

/**
 * Owning 2D image: geometry plus a contiguous pixel buffer.
 *
 * Coordinate and layout conventions:
 *   docs/architecture/coordinate-conventions.md
 *
 * Pixel type is double. Storage is row-major:
 *   linearIndex = j * width + i
 * for integer index (i, j) with i in [0, width) and j in [0, height).
 *
 * Geometry is fixed at construction. No I/O is performed by this type.
 */
class Image2D {
public:
    /**
     * Construct an image with the given geometry and zero-filled pixels.
     * @param geometry Image geometry (width and height must be >= 1).
     */
    explicit Image2D(ImageGeometry2D geometry)
        : m_geometry(std::move(geometry)), m_pixels(static_cast<std::size_t>(pixelCount()), 0.0) {}

    /**
     * Construct an image with geometry and an existing pixel buffer.
     * @param geometry Image geometry (width and height must be >= 1).
     * @param pixels Row-major buffer; size must equal width * height.
     */
    Image2D(ImageGeometry2D geometry, std::vector<double> pixels)
        : m_geometry(std::move(geometry)), m_pixels(std::move(pixels)) {
        assert(m_pixels.size() == static_cast<std::size_t>(pixelCount()));
    }

    /** @return Image geometry. */
    [[nodiscard]] const ImageGeometry2D& geometry() const noexcept { return m_geometry; }

    /** @return Number of pixels along the i axis. */
    [[nodiscard]] int width() const noexcept { return m_geometry.width(); }

    /** @return Number of pixels along the j axis. */
    [[nodiscard]] int height() const noexcept { return m_geometry.height(); }

    /** @return Total number of pixels (width * height). */
    [[nodiscard]] int pixelCount() const noexcept { return width() * height(); }

    /**
     * @param i Index along width.
     * @param j Index along height.
     * @return True if (i, j) lies inside the valid lattice.
     */
    [[nodiscard]] bool containsIndex(int i, int j) const noexcept {
        return m_geometry.containsIndex(i, j);
    }

    /**
     * Read a pixel at integer index (i, j).
     * @param i Index along width (must be in range).
     * @param j Index along height (must be in range).
     * @return Pixel value.
     */
    [[nodiscard]] double at(int i, int j) const {
        assert(containsIndex(i, j));
        return m_pixels[linearIndex(i, j)];
    }

    /**
     * Writable reference to a pixel at integer index (i, j).
     * @param i Index along width (must be in range).
     * @param j Index along height (must be in range).
     * @return Reference to the pixel value.
     */
    [[nodiscard]] double& at(int i, int j) {
        assert(containsIndex(i, j));
        return m_pixels[linearIndex(i, j)];
    }

    /** @see at(int, int) const */
    [[nodiscard]] double operator()(int i, int j) const { return at(i, j); }

    /** @see at(int, int) */
    [[nodiscard]] double& operator()(int i, int j) { return at(i, j); }

    /**
     * Set every pixel to the same value.
     * @param value Value written to all pixels.
     */
    void fill(double value) {
        for (double& pixel : m_pixels) {
            pixel = value;
        }
    }

    /**
     * @return Pointer to the first pixel in the row-major buffer (const).
     */
    [[nodiscard]] const double* data() const noexcept {
        return m_pixels.empty() ? nullptr : m_pixels.data();
    }

    /**
     * @return Pointer to the first pixel in the row-major buffer.
     */
    [[nodiscard]] double* data() noexcept { return m_pixels.empty() ? nullptr : m_pixels.data(); }

    /** @return Number of elements in the pixel buffer (same as pixelCount). */
    [[nodiscard]] std::size_t dataSize() const noexcept { return m_pixels.size(); }

private:
    [[nodiscard]] std::size_t linearIndex(int i, int j) const noexcept {
        return static_cast<std::size_t>(j * width() + i);
    }

    ImageGeometry2D m_geometry;
    std::vector<double> m_pixels;
};

} // namespace ir
