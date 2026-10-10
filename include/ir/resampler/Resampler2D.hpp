#pragma once

#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/transform/Transform2D.hpp>

namespace ir {

/**
 * Pure abstract interface for resampling a moving image into fixed geometry.
 *
 * Typical pipeline for each fixed pixel center (integer index):
 *   1. physical_fixed = fixedGeometry.indexToPhysical(i, j)
 *   2. physical_moving = transform.transformPoint(physical_fixed)
 *   3. continuous_index_moving = moving.geometry().physicalToIndex(physical_moving)
 *   4. value = interpolator.evaluate(moving, continuous_index_moving)
 *
 * Transform direction (v0.1, locked):
 * - transform maps **fixed physical → moving physical**
 *   (point in the fixed image's physical space to the corresponding point in
 *   the moving image's physical space).
 * - This is the mapping used when pulling samples from the moving image while
 *   iterating over the fixed grid.
 *
 * Ownership: polymorphic via base pointers/references; virtual destructor.
 * Production concrete resamplers are out of scope for this interface task.
 */
class Resampler2D {
public:
    virtual ~Resampler2D() = default;

    /**
     * Resample the moving image onto the fixed geometry.
     * @param moving Source image (pixels + geometry).
     * @param fixedGeometry Target lattice and physical mapping.
     * @param transform Maps fixed physical points to moving physical points.
     * @param interpolator Samples the moving image at continuous indices.
     * @return New Image2D with fixedGeometry and resampled intensities.
     */
    [[nodiscard]] virtual Image2D resample(const Image2D& moving,
                                           const ImageGeometry2D& fixedGeometry,
                                           const Transform2D& transform,
                                           const Interpolator2D& interpolator) const = 0;

protected:
    Resampler2D() = default;
    Resampler2D(const Resampler2D&) = default;
    Resampler2D& operator=(const Resampler2D&) = default;
    Resampler2D(Resampler2D&&) = default;
    Resampler2D& operator=(Resampler2D&&) = default;
};

} // namespace ir
