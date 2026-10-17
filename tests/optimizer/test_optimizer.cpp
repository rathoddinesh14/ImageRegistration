#include <ir/core/ErrorCode.hpp>
#include <ir/optimizer/CostFunction.hpp>
#include <ir/optimizer/Optimizer.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

/** Test-only: f(p) = (p[0] - 3)^2 ; one parameter. */
class QuadraticCostStub final : public ir::CostFunction {
public:
    [[nodiscard]] int numberOfParameters() const override { return 1; }

    [[nodiscard]] double value(const double* parameters) const override {
        const double d = parameters[0] - 3.0;
        return d * d;
    }
};

/** Test-only: reports success with cost at current params. */
class IdentityOptimizerStub final : public ir::Optimizer {
public:
    [[nodiscard]] ir::RegistrationResult optimize(const ir::CostFunction& cost,
                                                  double* parameters) const override {
        return ir::RegistrationResult::success(cost.value(parameters), 0);
    }
};

/** Test-only: always fails. */
class FailOptimizerStub final : public ir::Optimizer {
public:
    [[nodiscard]] ir::RegistrationResult optimize(const ir::CostFunction& /*cost*/,
                                                  double* /*parameters*/) const override {
        return ir::RegistrationResult::failure(ir::ErrorCode::Unknown, "stub fail");
    }
};

} // namespace

TEST_CASE("CostFunction is an abstract polymorphic base", "[optimizer][CostFunction]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::CostFunction>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::CostFunction>);
}

TEST_CASE("Optimizer is an abstract polymorphic base", "[optimizer][Optimizer]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::Optimizer>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::Optimizer>);
}

TEST_CASE("QuadraticCostStub evaluates known values", "[optimizer][CostFunction]") {
    const QuadraticCostStub cost;
    REQUIRE(cost.numberOfParameters() == 1);
    double p0 = 3.0;
    REQUIRE_THAT(cost.value(&p0), WithinAbs(0.0, kTol));
    double p1 = 5.0;
    REQUIRE_THAT(cost.value(&p1), WithinAbs(4.0, kTol));
}

TEST_CASE("IdentityOptimizerStub returns success with current cost", "[optimizer][Optimizer]") {
    const QuadraticCostStub cost;
    double params[] = {5.0};
    const IdentityOptimizerStub opt;

    const ir::RegistrationResult r = opt.optimize(cost, params);

    REQUIRE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Ok);
    REQUIRE_THAT(r.finalMetric, WithinAbs(4.0, kTol));
    REQUIRE(r.iterations == 0);
    REQUIRE_THAT(params[0], WithinAbs(5.0, kTol));
}

TEST_CASE("FailOptimizerStub returns failure", "[optimizer][Optimizer]") {
    const QuadraticCostStub cost;
    double params[] = {0.0};
    const FailOptimizerStub opt;

    const ir::RegistrationResult r = opt.optimize(cost, params);

    REQUIRE_FALSE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Unknown);
    REQUIRE(r.message == "stub fail");
}

TEST_CASE("Optimizer works through base reference", "[optimizer][Optimizer]") {
    const QuadraticCostStub cost;
    double params[] = {3.0};
    const IdentityOptimizerStub concrete;
    const ir::Optimizer& base = concrete;

    const ir::RegistrationResult r = base.optimize(cost, params);
    REQUIRE(r.succeeded);
    REQUIRE_THAT(r.finalMetric, WithinAbs(0.0, kTol));
}

TEST_CASE("Optimizer unique_ptr ownership", "[optimizer][Optimizer]") {
    const QuadraticCostStub cost;
    double params[] = {4.0};
    auto opt = std::make_unique<IdentityOptimizerStub>();

    const ir::RegistrationResult r = opt->optimize(cost, params);
    REQUIRE(r.succeeded);
    REQUIRE_THAT(r.finalMetric, WithinAbs(1.0, kTol));
}
