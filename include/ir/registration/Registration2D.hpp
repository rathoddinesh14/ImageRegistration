#pragma once

#include <ir/core/Image2D.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <ir/transform/Transform2D.hpp>

namespace ir {

/**
 * Pure abstract interface for a 2D image registration method.
 *
 * A registration run takes a fixed image, a moving image, and a transform
 * that maps fixed physical coordinates to moving physical coordinates
 * (same direction as Resampler2D). The transform is owned by the caller and
 * is updated in place with the estimated parameters when the run succeeds.
 *
 * Typical pipeline (implemented by concrete types, not this interface):
 *   resample moving under transform → evaluate metric → update transform
 *   via an optimizer, repeat until convergence or failure.
 *
 * Returns RegistrationResult (success flag, ErrorCode, final metric,
 * iterations, optional message). Does not throw for ordinary solve outcomes.
 *
 * No production algorithm is provided by this interface task; use stubs in
 * tests.
 */
class Registration2D {
public:
    virtual ~Registration2D() = default;

    /**
     * Run registration of moving onto fixed under the given transform model.
     *
     * @param fixed Reference (fixed) image.
     * @param moving Moving image to align to fixed.
     * @param transform In/out transform (fixed physical → moving physical).
     *        On success, holds the estimated parameters; on failure, contents
     *        are implementation-defined (may be partially updated).
     * @return Solve summary; check RegistrationResult::succeeded and code.
     */
    [[nodiscard]] virtual RegistrationResult run(const Image2D& fixed, const Image2D& moving,
                                                 Transform2D& transform) const = 0;

protected:
    Registration2D() = default;
    Registration2D(const Registration2D&) = default;
    Registration2D& operator=(const Registration2D&) = default;
    Registration2D(Registration2D&&) = default;
    Registration2D& operator=(Registration2D&&) = default;
};

} // namespace ir
