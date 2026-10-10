#pragma once

#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/resampler/Resampler2D.hpp>
#include <ir/transform/Transform2D.hpp>

namespace ir {

/**
 * Default resampler: pull samples from the moving image onto fixed geometry.
 *
 * For each fixed integer index (i, j):
 *   physical_fixed = fixedGeometry.indexToPhysical(i, j)
 *   physical_moving = transform.transformPoint(physical_fixed)
 *   continuous_index = moving.geometry().physicalToIndex(physical_moving)
 *   output(i, j) = interpolator.evaluate(moving, continuous_index)
 *
 * Transform maps fixed physical → moving physical (see Resampler2D).
 */
class ImageResampler2D final : public Resampler2D {
public:
    ImageResampler2D() = default;

    /**
     * Resample the moving image onto the fixed geometry.
     * @param moving Source image.
     * @param fixedGeometry Target lattice and physical mapping.
     * @param transform Maps fixed physical points to moving physical points.
     * @param interpolator Samples the moving image at continuous indices.
     * @return New Image2D with fixedGeometry and resampled intensities.
     */
    [[nodiscard]] Image2D resample(const Image2D& moving, const ImageGeometry2D& fixedGeometry,
                                   const Transform2D& transform,
                                   const Interpolator2D& interpolator) const override {
        Image2D output{fixedGeometry};
        const ImageGeometry2D& movingGeom = moving.geometry();
        const int w = fixedGeometry.width();
        const int h = fixedGeometry.height();

        for (int j = 0; j < h; ++j) {
            for (int i = 0; i < w; ++i) {
                const Point2D physicalFixed = fixedGeometry.indexToPhysical(i, j);
                const Point2D physicalMoving = transform.transformPoint(physicalFixed);
                const Point2D continuousIndex = movingGeom.physicalToIndex(physicalMoving);
                output.at(i, j) = interpolator.evaluate(moving, continuousIndex);
            }
        }
        return output;
    }
};

} // namespace ir
