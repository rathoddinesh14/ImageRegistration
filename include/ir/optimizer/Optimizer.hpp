#pragma once

#include <ir/optimizer/CostFunction.hpp>
#include <ir/registration/RegistrationResult.hpp>

namespace ir {

/**
 * Pure abstract iterative optimizer over a CostFunction parameter vector.
 *
 * Lifecycle:
 * 1. Caller allocates/fills an initial parameter vector of length
 *    cost.numberOfParameters().
 * 2. optimize() updates parameters in place and returns a RegistrationResult
 *    (success, final cost as finalMetric, iteration count, optional message).
 * 3. On failure, parameters may be partially updated (implementation-defined).
 *
 * Configuration (max iterations, step size, tolerance) belongs on concrete
 * optimizers, not this base interface.
 *
 * No production algorithm is provided by this interface task; use stubs in
 * tests. Parametric Transform2D adapters are a separate backlog item.
 */
class Optimizer {
public:
    virtual ~Optimizer() = default;

    /**
     * Optimize the cost with respect to parameters.
     * @param cost Cost function (value-only for v0.1).
     * @param parameters In/out parameter vector; length must equal
     *        cost.numberOfParameters(). Updated in place on success.
     * @return Solve summary; finalMetric is the final cost value.
     */
    [[nodiscard]] virtual RegistrationResult optimize(const CostFunction& cost,
                                                      double* parameters) const = 0;

protected:
    Optimizer() = default;
    Optimizer(const Optimizer&) = default;
    Optimizer& operator=(const Optimizer&) = default;
    Optimizer(Optimizer&&) = default;
    Optimizer& operator=(Optimizer&&) = default;
};

} // namespace ir
