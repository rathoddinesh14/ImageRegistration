#pragma once

#include <cmath>
#include <ir/core/Image2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/BoundsPolicy.hpp>
#include <ir/interpolator/Interpolator2D.hpp>

namespace ir {

/**
 * Nearest-neighbor sampling at continuous indices.
 *
 * Rounds continuous (i, j) to the nearest integer lattice point (pixel center).
 * Halfway cases follow std::lround (ties away from zero in magnitude, platform
 * lround behavior).
 *
 * Out-of-bounds:
 * - BoundsPolicy::Constant — return fillValue (default 0.0)
 * - BoundsPolicy::Clamp — sample the nearest in-bounds integer index
 */
class NearestNeighborInterpolator2D final : public Interpolator2D {
public:
    /**
     * @param policy Out-of-bounds policy (default Constant).
     * @param fillValue Value used when policy is Constant and sample is OOB.
     */
    explicit NearestNeighborInterpolator2D(BoundsPolicy policy = BoundsPolicy::Constant,
                                           double fillValue = 0.0) noexcept
        : m_policy(policy), m_fillValue(fillValue) {}

    /** @return Configured bounds policy. */
    [[nodiscard]] BoundsPolicy boundsPolicy() const noexcept { return m_policy; }

    /** @return Fill value for Constant policy. */
    [[nodiscard]] double fillValue() const noexcept { return m_fillValue; }

    /**
     * Sample the image at a continuous index by nearest neighbor.
     * @param image Source image.
     * @param continuousIndex Continuous (i, j) in index space.
     * @return Nearest pixel value, or policy-dependent OOB result.
     */
    [[nodiscard]] double evaluate(const Image2D& image,
                                  const Point2D& continuousIndex) const override {
        int i = static_cast<int>(std::lround(continuousIndex.x()));
        int j = static_cast<int>(std::lround(continuousIndex.y()));

        if (image.containsIndex(i, j)) {
            return image.at(i, j);
        }

        switch (m_policy) {
        case BoundsPolicy::Constant:
            return m_fillValue;
        case BoundsPolicy::Clamp: {
            const int w = image.width();
            const int h = image.height();
            if (i < 0) {
                i = 0;
            } else if (i >= w) {
                i = w - 1;
            }
            if (j < 0) {
                j = 0;
            } else if (j >= h) {
                j = h - 1;
            }
            return image.at(i, j);
        }
        }
        return m_fillValue;
    }

private:
    BoundsPolicy m_policy = BoundsPolicy::Constant;
    double m_fillValue = 0.0;
};

} // namespace ir
