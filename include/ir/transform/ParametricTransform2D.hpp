#pragma once

#include <ir/transform/Transform2D.hpp>

namespace ir {

/**
 * Transform that exposes a flat double parameter vector for optimizers.
 *
 * Sub-interface of Transform2D so pure geometric types can stay free of
 * parameter packing. Layout is type-specific and documented on each concrete
 * class (e.g. Translation2D: [tx, ty]; Rigid2D: [theta, tx, ty]).
 *
 * Caller owns the buffer of length numberOfParameters(). No bounds checking
 * beyond that contract — invalid length is undefined behavior (same style as
 * CostFunction::value).
 */
class ParametricTransform2D : public Transform2D {
public:
    ~ParametricTransform2D() override = default;

    /**
     * @return Number of parameters in get/setParameters buffers.
     */
    [[nodiscard]] virtual int numberOfParameters() const = 0;

    /**
     * Write current parameters into a contiguous buffer.
     * @param parameters Output array of length numberOfParameters().
     */
    virtual void getParameters(double* parameters) const = 0;

    /**
     * Replace parameters from a contiguous buffer.
     * @param parameters Input array of length numberOfParameters().
     */
    virtual void setParameters(const double* parameters) = 0;

protected:
    ParametricTransform2D() = default;
    ParametricTransform2D(const ParametricTransform2D&) = default;
    ParametricTransform2D& operator=(const ParametricTransform2D&) = default;
    ParametricTransform2D(ParametricTransform2D&&) = default;
    ParametricTransform2D& operator=(ParametricTransform2D&&) = default;
};

} // namespace ir
