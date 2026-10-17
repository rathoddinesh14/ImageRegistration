#pragma once

#include <ir/core/Point2D.hpp>
#include <ir/transform/ParametricTransform2D.hpp>

namespace ir {

/**
 * 2D translation transform: p' = p + offset.
 *
 * Parameter vector (numberOfParameters() == 2):
 *   [0] = offset.x (tx)
 *   [1] = offset.y (ty)
 *
 * Always invertible (inverse is translation by -offset; materializing the
 * inverse object is deferred until ir::Expected is available).
 */
class Translation2D final : public ParametricTransform2D {
public:
    /** Construct a zero translation (identity on points). */
    Translation2D() noexcept = default;

    /**
     * Construct a translation by the given offset.
     * @param offset Displacement added to each point (x, y).
     */
    explicit Translation2D(Point2D offset) noexcept : m_offset(offset) {}

    /**
     * @return The translation offset.
     */
    [[nodiscard]] Point2D offset() const noexcept { return m_offset; }

    /**
     * Apply the translation.
     * @param p Input point.
     * @return p + offset.
     */
    [[nodiscard]] Point2D transformPoint(const Point2D& p) const override { return p + m_offset; }

    /**
     * Translations are always invertible.
     * @return Always true.
     */
    [[nodiscard]] bool isInvertible() const noexcept override { return true; }

    /** @return Always 2 (tx, ty). */
    [[nodiscard]] int numberOfParameters() const override { return 2; }

    /**
     * Write [tx, ty] into parameters.
     * @param parameters Output length 2.
     */
    void getParameters(double* parameters) const override {
        parameters[0] = m_offset.x();
        parameters[1] = m_offset.y();
    }

    /**
     * Set offset from [tx, ty].
     * @param parameters Input length 2.
     */
    void setParameters(const double* parameters) override {
        m_offset = Point2D{parameters[0], parameters[1]};
    }

private:
    Point2D m_offset{};
};

} // namespace ir
