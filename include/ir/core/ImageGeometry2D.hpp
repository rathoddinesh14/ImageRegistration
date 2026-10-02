#pragma once

#include <cassert>
#include <ir/core/Point2D.hpp>
#include <utility>

#include <Eigen/Core>
#include <Eigen/LU>

namespace ir {

/**
 * Geometric properties of a 2D image (no pixel data).
 *
 * Coordinate conventions:
 * - Index space uses integer pixel indices (i, j) with i in [0, width) and
 *   j in [0, height).
 * - Origin is the physical position of the center of pixel (0, 0).
 * - Spacing is the physical size of one index step along each local axis.
 * - Direction is a 2x2 matrix whose columns are the local axis directions
 *   in physical space (identity = axis-aligned).
 *
 * Mapping (pixel center of integer index (i, j)):
 *   physical = origin + direction * (i * spacing.x(), j * spacing.y())
 *
 * Empty geometry is not allowed: width and height must be >= 1.
 */
class ImageGeometry2D {
public:
    /**
     * Construct geometry with identity direction.
     * @param width  Number of pixels along i (must be >= 1).
     * @param height Number of pixels along j (must be >= 1).
     * @param spacing Physical size per index step (x along i, y along j).
     * @param origin Physical coordinates of the center of pixel (0, 0).
     */
    ImageGeometry2D(int width, int height, Point2D spacing, Point2D origin)
        : ImageGeometry2D(width, height, spacing, origin, Eigen::Matrix2d::Identity()) {}

    /**
     * Construct geometry with an explicit direction matrix.
     * @param width  Number of pixels along i (must be >= 1).
     * @param height Number of pixels along j (must be >= 1).
     * @param spacing Physical size per index step (x along i, y along j).
     * @param origin Physical coordinates of the center of pixel (0, 0).
     * @param direction 2x2 direction matrix (columns = local axes in physical space).
     */
    ImageGeometry2D(int width, int height, Point2D spacing, Point2D origin,
                    Eigen::Matrix2d direction)
        : m_width(width), m_height(height), m_spacing(spacing), m_origin(origin),
          m_direction(std::move(direction)) {
        assert(m_width >= 1);
        assert(m_height >= 1);
    }

    /** @return Number of pixels along the i axis. */
    [[nodiscard]] int width() const noexcept { return m_width; }

    /** @return Number of pixels along the j axis. */
    [[nodiscard]] int height() const noexcept { return m_height; }

    /** @return Physical spacing per index step (x along i, y along j). */
    [[nodiscard]] Point2D spacing() const noexcept { return m_spacing; }

    /** @return Physical coordinates of the center of pixel (0, 0). */
    [[nodiscard]] Point2D origin() const noexcept { return m_origin; }

    /** @return 2x2 direction matrix (columns = local axes in physical space). */
    [[nodiscard]] const Eigen::Matrix2d& direction() const noexcept { return m_direction; }

    /**
     * Map an integer pixel index to physical coordinates (pixel center).
     * @param i Index along width (typically in [0, width)).
     * @param j Index along height (typically in [0, height)).
     * @return Physical coordinates of the center of pixel (i, j).
     */
    [[nodiscard]] Point2D indexToPhysical(int i, int j) const {
        const Eigen::Vector2d local{static_cast<double>(i) * m_spacing.x(),
                                    static_cast<double>(j) * m_spacing.y()};
        const Eigen::Vector2d physical = m_origin.toEigen() + m_direction * local;
        return Point2D::fromEigen(physical);
    }

    /**
     * Map physical coordinates to continuous index space.
     * @param physical Point in physical space.
     * @return Continuous indices (i, j); may be fractional between pixel centers.
     */
    [[nodiscard]] Point2D physicalToIndex(const Point2D& physical) const {
        const Eigen::Vector2d delta = physical.toEigen() - m_origin.toEigen();
        const Eigen::Vector2d local = m_direction.inverse() * delta;
        return Point2D{local.x() / m_spacing.x(), local.y() / m_spacing.y()};
    }

    /**
     * Test whether an integer index lies inside the valid pixel lattice.
     * @param i Index along width.
     * @param j Index along height.
     * @return True if 0 <= i < width and 0 <= j < height.
     */
    [[nodiscard]] bool containsIndex(int i, int j) const noexcept {
        return i >= 0 && j >= 0 && i < m_width && j < m_height;
    }

private:
    int m_width = 1;
    int m_height = 1;
    Point2D m_spacing{1.0, 1.0};
    Point2D m_origin{};
    Eigen::Matrix2d m_direction{Eigen::Matrix2d::Identity()};
};

/**
 * Equality of size, spacing, origin, and direction (exact double compare).
 * @param a First geometry.
 * @param b Second geometry.
 * @return True if all stored fields compare equal.
 */
[[nodiscard]] inline bool operator==(const ImageGeometry2D& a, const ImageGeometry2D& b) {
    return a.width() == b.width() && a.height() == b.height() && a.spacing() == b.spacing() &&
           a.origin() == b.origin() && a.direction() == b.direction();
}

/**
 * Inequality comparison.
 * @param a First geometry.
 * @param b Second geometry.
 * @return True if any stored field differs.
 */
[[nodiscard]] inline bool operator!=(const ImageGeometry2D& a, const ImageGeometry2D& b) {
    return !(a == b);
}

} // namespace ir
