#pragma once

#include <ir/core/Image2D.hpp>
#include <ir/interpolator/BoundsPolicy.hpp>

namespace ir {

/**
 * Read one integer lattice sample with a shared out-of-bounds policy.
 *
 * Used by nearest-neighbor and bilinear interpolators so Constant/Clamp
 * behavior stays consistent.
 *
 * @param image Source image.
 * @param i Integer index along width.
 * @param j Integer index along height.
 * @param policy Bounds policy.
 * @param fillValue Value when policy is Constant and (i, j) is outside.
 * @return Pixel value or policy-dependent substitute.
 */
[[nodiscard]] inline double sampleAt(const Image2D& image, int i, int j, BoundsPolicy policy,
                                     double fillValue) {
    if (image.containsIndex(i, j)) {
        return image.at(i, j);
    }
    switch (policy) {
    case BoundsPolicy::Constant:
        return fillValue;
    case BoundsPolicy::Clamp: {
        const int w = image.width();
        const int h = image.height();
        int ci = i;
        int cj = j;
        if (ci < 0) {
            ci = 0;
        } else if (ci >= w) {
            ci = w - 1;
        }
        if (cj < 0) {
            cj = 0;
        } else if (cj >= h) {
            cj = h - 1;
        }
        return image.at(ci, cj);
    }
    }
    return fillValue;
}

} // namespace ir
