#pragma once

#include <cmath>
#include <ir/core/Image2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/BoundsPolicy.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/interpolator/sampleAt.hpp>

namespace ir {

/**
 * Bilinear interpolation at continuous indices.
 *
 * For continuous index (x, y), uses the unit square with corners
 * (floor(x), floor(y)), (floor(x)+1, floor(y)), etc., and standard bilinear
 * weights. Integer lattice points are pixel centers.
 *
 * Out-of-bounds (per corner): see BoundsPolicy and sampleAt().
 */
class LinearInterpolator2D final : public Interpolator2D {
public:
    /**
     * @param policy Out-of-bounds policy (default Constant).
     * @param fillValue Value used when policy is Constant and a corner is OOB.
     */
    explicit LinearInterpolator2D(BoundsPolicy policy = BoundsPolicy::Constant,
                                  double fillValue = 0.0) noexcept
        : m_policy(policy), m_fillValue(fillValue) {}

    /** @return Configured bounds policy. */
    [[nodiscard]] BoundsPolicy boundsPolicy() const noexcept { return m_policy; }

    /** @return Fill value for Constant policy. */
    [[nodiscard]] double fillValue() const noexcept { return m_fillValue; }

    /**
     * Bilinear sample at a continuous index.
     * @param image Source image.
     * @param continuousIndex Continuous (i, j) in index space.
     * @return Interpolated intensity.
     */
    [[nodiscard]] double evaluate(const Image2D& image,
                                  const Point2D& continuousIndex) const override {
        const double x = continuousIndex.x();
        const double y = continuousIndex.y();

        const int i0 = static_cast<int>(std::floor(x));
        const int j0 = static_cast<int>(std::floor(y));
        const int i1 = i0 + 1;
        const int j1 = j0 + 1;

        const double fx = x - static_cast<double>(i0);
        const double fy = y - static_cast<double>(j0);

        const double v00 = sampleAt(image, i0, j0, m_policy, m_fillValue);
        const double v10 = sampleAt(image, i1, j0, m_policy, m_fillValue);
        const double v01 = sampleAt(image, i0, j1, m_policy, m_fillValue);
        const double v11 = sampleAt(image, i1, j1, m_policy, m_fillValue);

        const double v0 = v00 * (1.0 - fx) + v10 * fx;
        const double v1 = v01 * (1.0 - fx) + v11 * fx;
        return v0 * (1.0 - fy) + v1 * fy;
    }

private:
    BoundsPolicy m_policy = BoundsPolicy::Constant;
    double m_fillValue = 0.0;
};

} // namespace ir
