#pragma once

#include <ir/core/ErrorCode.hpp>
#include <ir/core/Image2D.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/metric/Metric2D.hpp>
#include <ir/optimizer/CostFunction.hpp>
#include <ir/optimizer/Optimizer.hpp>
#include <ir/registration/Registration2D.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <ir/resampler/Resampler2D.hpp>
#include <ir/transform/ParametricTransform2D.hpp>
#include <ir/transform/Transform2D.hpp>
#include <vector>

namespace ir {

namespace detail {

/**
 * Internal cost: parameters → set transform → resample moving → metric.
 * Mutates the parametric transform during evaluation (optimizer contract).
 */
class RegistrationCost2D final : public CostFunction {
public:
    RegistrationCost2D(const Image2D& fixed, const Image2D& moving,
                       ParametricTransform2D& transform, const Metric2D& metric,
                       const Resampler2D& resampler, const Interpolator2D& interpolator)
        : m_fixed(&fixed), m_moving(&moving), m_transform(&transform), m_metric(&metric),
          m_resampler(&resampler), m_interpolator(&interpolator) {}

    [[nodiscard]] int numberOfParameters() const override {
        return m_transform->numberOfParameters();
    }

    [[nodiscard]] double value(const double* parameters) const override {
        m_transform->setParameters(parameters);
        const Image2D resampled =
            m_resampler->resample(*m_moving, m_fixed->geometry(), *m_transform, *m_interpolator);
        double cost = m_metric->evaluate(*m_fixed, resampled);
        if (!m_metric->isMinimize()) {
            cost = -cost;
        }
        return cost;
    }

private:
    const Image2D* m_fixed;
    const Image2D* m_moving;
    ParametricTransform2D* m_transform;
    const Metric2D* m_metric;
    const Resampler2D* m_resampler;
    const Interpolator2D* m_interpolator;
};

} // namespace detail

/**
 * Concrete 2D registration: metric + resampler + interpolator + optimizer.
 *
 * Components are non-owning (caller keeps them alive for the lifetime of
 * this method object and across run()). The transform argument to run() must
 * be a ParametricTransform2D; otherwise run() fails with NotImplemented.
 *
 * Pipeline per optimize iteration (via internal RegistrationCost2D):
 *   setParameters → resample moving onto fixed geometry → evaluate metric
 */
class ImageRegistrationMethod2D final : public Registration2D {
public:
    /**
     * @param metric Similarity/dissimilarity metric (e.g. mean squares).
     * @param resampler Pull resampler onto fixed geometry.
     * @param interpolator Sampling policy for the resampler.
     * @param optimizer Parameter search (e.g. RegularStepGradientDescent).
     */
    ImageRegistrationMethod2D(const Metric2D& metric, const Resampler2D& resampler,
                              const Interpolator2D& interpolator, const Optimizer& optimizer)
        : m_metric(&metric), m_resampler(&resampler), m_interpolator(&interpolator),
          m_optimizer(&optimizer) {}

    /**
     * Register moving onto fixed by optimizing transform parameters.
     * @param fixed Reference image.
     * @param moving Moving image.
     * @param transform In/out; must be ParametricTransform2D (updated in place).
     * @return Optimizer solve summary.
     */
    [[nodiscard]] RegistrationResult run(const Image2D& fixed, const Image2D& moving,
                                         Transform2D& transform) const override {
        auto* parametric = dynamic_cast<ParametricTransform2D*>(&transform);
        if (parametric == nullptr) {
            return RegistrationResult::failure(ErrorCode::NotImplemented,
                                               "transform is not ParametricTransform2D");
        }

        const int n = parametric->numberOfParameters();
        if (n <= 0) {
            return RegistrationResult::failure(ErrorCode::InvalidGeometry,
                                               "empty parameter vector");
        }

        std::vector<double> parameters(static_cast<std::size_t>(n));
        parametric->getParameters(parameters.data());

        const detail::RegistrationCost2D cost(fixed, moving, *parametric, *m_metric, *m_resampler,
                                              *m_interpolator);
        const RegistrationResult result = m_optimizer->optimize(cost, parameters.data());
        parametric->setParameters(parameters.data());
        return result;
    }

private:
    const Metric2D* m_metric;
    const Resampler2D* m_resampler;
    const Interpolator2D* m_interpolator;
    const Optimizer* m_optimizer;
};

} // namespace ir
