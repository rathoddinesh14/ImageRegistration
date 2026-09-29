#pragma once

#include <Eigen/Core>

namespace ir {

/// A 2D point in continuous coordinates.
///
/// Value type with no dynamic allocation. Suitable for geometry, sampling,
/// and transform application. Interoperates with Eigen via toEigen/fromEigen.
class Point2D {
public:
    /// Construct the origin (0, 0).
    Point2D() noexcept = default;

    /// Construct a point from Cartesian coordinates.
    /// @param x Horizontal coordinate.
    /// @param y Vertical coordinate.
    Point2D(double x, double y) noexcept
        : m_x(x), m_y(y) {}

    /// @return The x-coordinate.
    [[nodiscard]] double x() const noexcept { return m_x; }

    /// @return The y-coordinate.
    [[nodiscard]] double y() const noexcept { return m_y; }

    /// Add another point componentwise (vector-style).
    /// @param other Point to add.
    /// @return Reference to this point after update.
    Point2D& operator+=(const Point2D& other) noexcept {
        m_x += other.m_x;
        m_y += other.m_y;
        return *this;
    }

    /// Subtract another point componentwise (vector-style).
    /// @param other Point to subtract.
    /// @return Reference to this point after update.
    Point2D& operator-=(const Point2D& other) noexcept {
        m_x -= other.m_x;
        m_y -= other.m_y;
        return *this;
    }

    /// Unary minus: negate both coordinates.
    /// @return A new point with negated coordinates.
    [[nodiscard]] Point2D operator-() const noexcept { return Point2D{-m_x, -m_y}; }

    /// Convert to Eigen column vector.
    /// @return Eigen::Vector2d with the same coordinates.
    [[nodiscard]] Eigen::Vector2d toEigen() const noexcept { return Eigen::Vector2d{m_x, m_y}; }

    /// Construct from an Eigen column vector.
    /// @param v Source vector (x = v.x(), y = v.y()).
    /// @return Point with the same coordinates.
    [[nodiscard]] static Point2D fromEigen(const Eigen::Vector2d& v) noexcept {
        return Point2D{v.x(), v.y()};
    }

private:
    double m_x = 0.0;
    double m_y = 0.0;
};

/// Componentwise addition.
/// @param lhs Left point (taken by value).
/// @param rhs Right point.
/// @return Sum of the two points.
[[nodiscard]] inline Point2D operator+(Point2D lhs, const Point2D& rhs) noexcept {
    lhs += rhs;
    return lhs;
}

/// Componentwise subtraction.
/// @param lhs Left point (taken by value).
/// @param rhs Right point.
/// @return Difference of the two points.
[[nodiscard]] inline Point2D operator-(Point2D lhs, const Point2D& rhs) noexcept {
    lhs -= rhs;
    return lhs;
}

/// Scale a point by a scalar.
/// @param p Point to scale.
/// @param s Scale factor.
/// @return Scaled point.
[[nodiscard]] inline Point2D operator*(Point2D p, double s) noexcept {
    return Point2D{p.x() * s, p.y() * s};
}

/// Scale a point by a scalar (commutative form).
/// @param s Scale factor.
/// @param p Point to scale.
/// @return Scaled point.
[[nodiscard]] inline Point2D operator*(double s, Point2D p) noexcept {
    return p * s;
}

/// Equality comparison (exact double compare).
/// @param a First point.
/// @param b Second point.
/// @return True if both coordinates are equal.
[[nodiscard]] inline bool operator==(const Point2D& a, const Point2D& b) noexcept {
    return a.x() == b.x() && a.y() == b.y();
}

/// Inequality comparison.
/// @param a First point.
/// @param b Second point.
/// @return True if any coordinate differs.
[[nodiscard]] inline bool operator!=(const Point2D& a, const Point2D& b) noexcept {
    return !(a == b);
}

} // namespace ir
