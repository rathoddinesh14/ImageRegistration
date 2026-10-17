#pragma once

#include <cmath>
#include <ir/core/Point2D.hpp>
#include <ir/transform/ParametricTransform2D.hpp>

namespace ir {

/**
 * 2D rigid transform (SE(2)): rotate about the physical origin, then translate.
 *
 * Mapping:
 *   p' = R(theta) * p + t
 * where R is the standard 2D rotation matrix:
 *   [ cos(theta)  -sin(theta) ]
 *   [ sin(theta)   cos(theta) ]
 *
 * Parameter vector (numberOfParameters() == 3):
 *   [0] = angle in radians (theta)
 *   [1] = translation.x (tx)
 *   [2] = translation.y (ty)
 *
 * Angle is in radians. isInvertible() is always true; materializing the inverse
 * object is deferred until ir::Expected is available.
 */
class Rigid2D final : public ParametricTransform2D {
public:
    /** Identity: zero angle and zero translation. */
    Rigid2D() noexcept = default;

    /**
     * Construct a rigid motion.
     * @param angleRadians Rotation angle in radians (counter-clockwise).
     * @param translation Translation applied after rotation.
     */
    Rigid2D(double angleRadians, Point2D translation) noexcept
        : m_angle(angleRadians), m_translation(translation) {}

    /** @return Rotation angle in radians. */
    [[nodiscard]] double angle() const noexcept { return m_angle; }

    /** @return Translation applied after rotation. */
    [[nodiscard]] Point2D translation() const noexcept { return m_translation; }

    /**
     * Apply rigid mapping: rotate about origin, then translate.
     * @param p Input point in physical coordinates.
     * @return Transformed point.
     */
    [[nodiscard]] Point2D transformPoint(const Point2D& p) const override {
        const double c = std::cos(m_angle);
        const double s = std::sin(m_angle);
        const double x = c * p.x() - s * p.y() + m_translation.x();
        const double y = s * p.x() + c * p.y() + m_translation.y();
        return Point2D{x, y};
    }

    /**
     * Rigid motions are always invertible.
     * @return Always true.
     */
    [[nodiscard]] bool isInvertible() const noexcept override { return true; }

    /** @return Always 3 (theta, tx, ty). */
    [[nodiscard]] int numberOfParameters() const override { return 3; }

    /**
     * Write [theta, tx, ty] into parameters.
     * @param parameters Output length 3.
     */
    void getParameters(double* parameters) const override {
        parameters[0] = m_angle;
        parameters[1] = m_translation.x();
        parameters[2] = m_translation.y();
    }

    /**
     * Set angle and translation from [theta, tx, ty].
     * @param parameters Input length 3.
     */
    void setParameters(const double* parameters) override {
        m_angle = parameters[0];
        m_translation = Point2D{parameters[1], parameters[2]};
    }

private:
    double m_angle = 0.0;
    Point2D m_translation{};
};

} // namespace ir
