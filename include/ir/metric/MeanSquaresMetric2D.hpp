#pragma once

#include <cassert>
#include <ir/core/Image2D.hpp>
#include <ir/metric/Metric2D.hpp>

namespace ir {

/**
 * Mean squared intensity difference (MSE) between fixed and moving images.
 *
 * For v0.1, samples every pixel of the fixed image lattice and requires the
 * moving image to have the same width and height (asserted). The metric is:
 *   (1/N) * sum_i (fixed[i] - moving[i])^2
 *
 * isMinimize() is true (lower is better). Size mismatch / empty overlap via
 * ir::Expected is deferred.
 */
class MeanSquaresMetric2D final : public Metric2D {
public:
    MeanSquaresMetric2D() = default;

    /**
     * Mean squared difference over all pixels.
     * @param fixed Reference image.
     * @param moving Moving image; must match fixed width and height.
     * @return Non-negative MSE (0 if images are identical).
     */
    [[nodiscard]] double evaluate(const Image2D& fixed, const Image2D& moving) const override {
        assert(fixed.width() == moving.width());
        assert(fixed.height() == moving.height());

        const int w = fixed.width();
        const int h = fixed.height();
        const int n = w * h;
        assert(n > 0);

        double sumSq = 0.0;
        for (int j = 0; j < h; ++j) {
            for (int i = 0; i < w; ++i) {
                const double d = fixed.at(i, j) - moving.at(i, j);
                sumSq += d * d;
            }
        }
        return sumSq / static_cast<double>(n);
    }

    /**
     * MSE is a cost metric.
     * @return Always true (minimize).
     */
    [[nodiscard]] bool isMinimize() const noexcept override { return true; }
};

} // namespace ir
