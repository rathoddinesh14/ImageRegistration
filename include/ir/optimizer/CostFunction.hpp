#pragma once

namespace ir {

/**
 * Pure abstract cost (or similarity) evaluated at a parameter vector.
 *
 * Used by Optimizer. Concrete registration will typically wrap resample +
 * metric so that value(parameters) returns the current metric for those
 * transform parameters. Gradient-based optimizers may extend this later.
 *
 * Parameter layout is owned by the caller / transform model; this interface
 * only requires a contiguous double array of length numberOfParameters().
 */
class CostFunction {
public:
    virtual ~CostFunction() = default;

    /**
     * @return Number of parameters in the vector passed to value().
     */
    [[nodiscard]] virtual int numberOfParameters() const = 0;

    /**
     * Evaluate the cost at the given parameters.
     * @param parameters Contiguous array of length numberOfParameters().
     * @return Scalar cost (minimize) or negated similarity, per the metric.
     */
    [[nodiscard]] virtual double value(const double* parameters) const = 0;

protected:
    CostFunction() = default;
    CostFunction(const CostFunction&) = default;
    CostFunction& operator=(const CostFunction&) = default;
    CostFunction(CostFunction&&) = default;
    CostFunction& operator=(CostFunction&&) = default;
};

} // namespace ir
