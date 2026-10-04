#pragma once

#include <ir/core/Image2D.hpp>
#include <ir/core/Point2D.hpp>

namespace ir {

/**
 * Pure abstract interface for sampling an Image2D at continuous indices.
 *
 * Domain:
 * - continuousIndex.x() is continuous i (along width)
 * - continuousIndex.y() is continuous j (along height)
 * Integer lattice points (i, j) are pixel centers (see coordinate conventions).
 *
 * Out-of-bounds policy (v0.1):
 * - Concrete interpolators must document behavior when the sample falls outside
 *   the valid index domain for their kernel support.
 * - Recommended default for simple interpolators: return 0.0 when the evaluation
 *   location is outside the image lattice bounds used by that interpolator.
 * - Recoverable error signaling via ir::Expected is deferred.
 *
 * Ownership:
 * - Polymorphic use via base pointers/references; virtual destructor is public.
 * - Prefer unique_ptr<Interpolator2D> when owning a concrete interpolator.
 */
class Interpolator2D {
public:
    virtual ~Interpolator2D() = default;

    /**
     * Evaluate the image at a continuous index.
     * @param image Source image (geometry and pixels).
     * @param continuousIndex Continuous (i, j) in index space (pixel-center lattice).
     * @return Interpolated intensity (double).
     */
    [[nodiscard]] virtual double evaluate(const Image2D& image,
                                          const Point2D& continuousIndex) const = 0;

protected:
    Interpolator2D() = default;
    Interpolator2D(const Interpolator2D&) = default;
    Interpolator2D& operator=(const Interpolator2D&) = default;
    Interpolator2D(Interpolator2D&&) = default;
    Interpolator2D& operator=(Interpolator2D&&) = default;
};

} // namespace ir
