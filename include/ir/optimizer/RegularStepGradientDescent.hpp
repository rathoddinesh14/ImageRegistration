#pragma once

#include <cmath>
#include <ir/core/ErrorCode.hpp>
#include <ir/optimizer/CostFunction.hpp>
#include <ir/optimizer/Optimizer.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <vector>

namespace ir {

/**
 * Regular-step gradient descent using central finite differences.
 *
 * Each iteration:
 * 1. Estimate gradient g by central differences with epsilon.
 * 2. Propose p <- p - stepLength * g (raw gradient scale).
 * 3. If cost improves, accept and optionally grow stepLength (up to maxStep).
 * 4. Otherwise shrink stepLength and retry (up to a small attempt limit).
 *
 * Stops when gradient L2 norm is below gradientTolerance, stepLength falls
 * below minStep, maxIterations is reached, or a non-finite cost is seen.
 *
 * Limitations (v0.1):
 * - Not a production Ceres/ITK-quality solver.
 * - Gradients from CostFunction::value only (no analytic derivatives).
 * - Simple regular step; no trust region or advanced line search.
 * - Aimed at smooth/convex toy costs and small demos (e.g. Translation2D).
 * - Cost scales with parameter count (2 evaluations per parameter per gradient).
 */
class RegularStepGradientDescent final : public Optimizer {
public:
    /**
     * @param maxIterations Maximum outer iterations (default 100).
     * @param learningRate Initial step length scale (default 0.1).
     * @param gradientTolerance Stop when ||g||_2 is below this (default 1e-6).
     * @param minStep Stop when step length falls below this (default 1e-8).
     * @param maxStep Cap on step length growth (default 1.0).
     * @param relaxationFactor Multiply step on success (default 1.1).
     * @param shrinkFactor Multiply step on failure (default 0.5).
     * @param differenceEpsilon Central-difference half-step (default 1e-6).
     */
    explicit RegularStepGradientDescent(int maxIterations = 100, double learningRate = 0.1,
                                        double gradientTolerance = 1e-6, double minStep = 1e-8,
                                        double maxStep = 1.0, double relaxationFactor = 1.1,
                                        double shrinkFactor = 0.5,
                                        double differenceEpsilon = 1e-6) noexcept
        : m_maxIterations(maxIterations), m_learningRate(learningRate),
          m_gradientTolerance(gradientTolerance), m_minStep(minStep), m_maxStep(maxStep),
          m_relaxationFactor(relaxationFactor), m_shrinkFactor(shrinkFactor),
          m_differenceEpsilon(differenceEpsilon) {}

    [[nodiscard]] int maxIterations() const noexcept { return m_maxIterations; }
    [[nodiscard]] double learningRate() const noexcept { return m_learningRate; }
    [[nodiscard]] double gradientTolerance() const noexcept { return m_gradientTolerance; }

    /**
     * Run regular-step GD from the given parameters.
     * @param cost Cost to minimize.
     * @param parameters In/out vector of length cost.numberOfParameters().
     * @return RegistrationResult with final cost and iteration count.
     */
    [[nodiscard]] RegistrationResult optimize(const CostFunction& cost,
                                              double* parameters) const override {
        const int n = cost.numberOfParameters();
        if (n <= 0 || parameters == nullptr) {
            return RegistrationResult::failure(ErrorCode::InvalidGeometry,
                                               "empty parameter vector");
        }

        std::vector<double> gradient(static_cast<std::size_t>(n));
        std::vector<double> trial(static_cast<std::size_t>(n));
        std::vector<double> workspace(static_cast<std::size_t>(n));

        double stepLength = m_learningRate;
        double currentCost = cost.value(parameters);
        if (!std::isfinite(currentCost)) {
            return RegistrationResult::failure(ErrorCode::NonFiniteMetric,
                                               "non-finite cost at start");
        }

        int iterations = 0;
        for (; iterations < m_maxIterations; ++iterations) {
            computeGradient(cost, parameters, n, gradient.data(), workspace.data());
            const double gradNorm = l2Norm(gradient.data(), n);
            if (gradNorm < m_gradientTolerance) {
                return RegistrationResult::success(currentCost, iterations);
            }

            bool accepted = false;
            for (int attempt = 0; attempt < 8 && stepLength >= m_minStep; ++attempt) {
                for (int i = 0; i < n; ++i) {
                    trial[static_cast<std::size_t>(i)] =
                        parameters[i] - stepLength * gradient[static_cast<std::size_t>(i)];
                }
                const double trialCost = cost.value(trial.data());
                if (!std::isfinite(trialCost)) {
                    stepLength *= m_shrinkFactor;
                    continue;
                }
                if (trialCost < currentCost) {
                    for (int i = 0; i < n; ++i) {
                        parameters[i] = trial[static_cast<std::size_t>(i)];
                    }
                    currentCost = trialCost;
                    stepLength = std::min(stepLength * m_relaxationFactor, m_maxStep);
                    accepted = true;
                    break;
                }
                stepLength *= m_shrinkFactor;
            }

            if (!accepted) {
                if (stepLength < m_minStep) {
                    return RegistrationResult::success(currentCost, iterations + 1);
                }
                // No improvement after shrink attempts; stop with current best.
                return RegistrationResult::success(currentCost, iterations + 1);
            }
        }

        return RegistrationResult::success(currentCost, iterations);
    }

private:
    static double l2Norm(const double* v, int n) {
        double s = 0.0;
        for (int i = 0; i < n; ++i) {
            s += v[i] * v[i];
        }
        return std::sqrt(s);
    }

    void computeGradient(const CostFunction& cost, const double* parameters, int n,
                         double* gradient, double* workspace) const {
        const double eps = m_differenceEpsilon;
        for (int i = 0; i < n; ++i) {
            workspace[i] = parameters[i];
        }
        for (int i = 0; i < n; ++i) {
            workspace[i] = parameters[i] + eps;
            const double fPlus = cost.value(workspace);
            workspace[i] = parameters[i] - eps;
            const double fMinus = cost.value(workspace);
            workspace[i] = parameters[i];
            gradient[i] = (fPlus - fMinus) / (2.0 * eps);
        }
    }

    int m_maxIterations;
    double m_learningRate;
    double m_gradientTolerance;
    double m_minStep;
    double m_maxStep;
    double m_relaxationFactor;
    double m_shrinkFactor;
    double m_differenceEpsilon;
};

} // namespace ir
