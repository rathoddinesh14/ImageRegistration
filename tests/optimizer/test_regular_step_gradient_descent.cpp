#include <ir/core/ErrorCode.hpp>
#include <ir/optimizer/CostFunction.hpp>
#include <ir/optimizer/Optimizer.hpp>
#include <ir/optimizer/RegularStepGradientDescent.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <cmath>
#include <limits>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

/** f(p) = (p[0] - 3)^2 */
class Quadratic1D final : public ir::CostFunction {
public:
    [[nodiscard]] int numberOfParameters() const override { return 1; }

    [[nodiscard]] double value(const double* parameters) const override {
        const double d = parameters[0] - 3.0;
        return d * d;
    }
};

/** f(x,y) = (x-1)^2 + (y+2)^2 */
class Quadratic2D final : public ir::CostFunction {
public:
    [[nodiscard]] int numberOfParameters() const override { return 2; }

    [[nodiscard]] double value(const double* parameters) const override {
        const double dx = parameters[0] - 1.0;
        const double dy = parameters[1] + 2.0;
        return dx * dx + dy * dy;
    }
};

/** Always returns NaN. */
class NanCost final : public ir::CostFunction {
public:
    [[nodiscard]] int numberOfParameters() const override { return 1; }

    [[nodiscard]] double value(const double* /*parameters*/) const override {
        return std::numeric_limits<double>::quiet_NaN();
    }
};

} // namespace

TEST_CASE("RegularStepGradientDescent is a concrete Optimizer",
          "[optimizer][RegularStepGradientDescent]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Optimizer, ir::RegularStepGradientDescent>);
    STATIC_REQUIRE(std::is_final_v<ir::RegularStepGradientDescent>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::RegularStepGradientDescent>);
}

TEST_CASE("RegularStepGradientDescent minimizes 1D quadratic",
          "[optimizer][RegularStepGradientDescent]") {
    const Quadratic1D cost;
    double p[] = {5.0};
    const ir::RegularStepGradientDescent opt{200, 0.2};

    const ir::RegistrationResult r = opt.optimize(cost, p);

    REQUIRE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Ok);
    REQUIRE(r.finalMetric < 1e-6);
    REQUIRE_THAT(p[0], WithinAbs(3.0, 1e-3));
    REQUIRE(r.iterations >= 1);
}

TEST_CASE("RegularStepGradientDescent minimizes 2D quadratic",
          "[optimizer][RegularStepGradientDescent]") {
    const Quadratic2D cost;
    double p[] = {4.0, 3.0};
    const ir::RegularStepGradientDescent opt{300, 0.15};

    const ir::RegistrationResult r = opt.optimize(cost, p);

    REQUIRE(r.succeeded);
    REQUIRE(r.finalMetric < 1e-5);
    REQUIRE_THAT(p[0], WithinAbs(1.0, 1e-2));
    REQUIRE_THAT(p[1], WithinAbs(-2.0, 1e-2));
}

TEST_CASE("RegularStepGradientDescent fails on non-finite cost",
          "[optimizer][RegularStepGradientDescent]") {
    const NanCost cost;
    double p[] = {0.0};
    const ir::RegularStepGradientDescent opt;

    const ir::RegistrationResult r = opt.optimize(cost, p);

    REQUIRE_FALSE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::NonFiniteMetric);
}

TEST_CASE("RegularStepGradientDescent works through Optimizer base",
          "[optimizer][RegularStepGradientDescent]") {
    const Quadratic1D cost;
    double p[] = {0.0};
    const ir::RegularStepGradientDescent concrete{150, 0.25};
    const ir::Optimizer& base = concrete;

    const ir::RegistrationResult r = base.optimize(cost, p);
    REQUIRE(r.succeeded);
    REQUIRE_THAT(p[0], WithinAbs(3.0, 1e-2));
}
