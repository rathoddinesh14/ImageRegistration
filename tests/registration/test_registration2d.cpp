#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/registration/Registration2D.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <ir/transform/Transform2D.hpp>
#include <ir/transform/Translation2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

[[nodiscard]] ir::Image2D makeImage(int w, int h) {
    return ir::Image2D{ir::ImageGeometry2D{w, h, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}}};
}

/** Test-only: always succeeds with a fixed metric and iteration count. */
class SuccessRegistrationStub final : public ir::Registration2D {
public:
    [[nodiscard]] ir::RegistrationResult run(const ir::Image2D& /*fixed*/,
                                             const ir::Image2D& /*moving*/,
                                             ir::Transform2D& /*transform*/) const override {
        return ir::RegistrationResult::success(0.5, 7);
    }
};

/** Test-only: always fails with EmptyOverlap. */
class FailRegistrationStub final : public ir::Registration2D {
public:
    [[nodiscard]] ir::RegistrationResult run(const ir::Image2D& /*fixed*/,
                                             const ir::Image2D& /*moving*/,
                                             ir::Transform2D& /*transform*/) const override {
        return ir::RegistrationResult::failure(ir::ErrorCode::EmptyOverlap, "stub fail");
    }
};

} // namespace

TEST_CASE("Registration2D is an abstract polymorphic base", "[registration][Registration2D]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::Registration2D>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::Registration2D>);
}

TEST_CASE("Registration2D success stub returns success result", "[registration][Registration2D]") {
    const ir::Image2D fixed = makeImage(2, 2);
    const ir::Image2D moving = makeImage(2, 2);
    ir::Translation2D transform;
    const SuccessRegistrationStub method;

    const ir::RegistrationResult result = method.run(fixed, moving, transform);

    REQUIRE(result.succeeded);
    REQUIRE(result.code == ir::ErrorCode::Ok);
    REQUIRE_THAT(result.finalMetric, WithinAbs(0.5, kTol));
    REQUIRE(result.iterations == 7);
}

TEST_CASE("Registration2D failure stub returns failure result", "[registration][Registration2D]") {
    const ir::Image2D fixed = makeImage(2, 2);
    const ir::Image2D moving = makeImage(2, 2);
    ir::Translation2D transform;
    const FailRegistrationStub method;

    const ir::RegistrationResult result = method.run(fixed, moving, transform);

    REQUIRE_FALSE(result.succeeded);
    REQUIRE(result.code == ir::ErrorCode::EmptyOverlap);
    REQUIRE(result.message == "stub fail");
}

TEST_CASE("Registration2D works through base reference", "[registration][Registration2D]") {
    const ir::Image2D fixed = makeImage(1, 1);
    const ir::Image2D moving = makeImage(1, 1);
    ir::Translation2D transform;
    const SuccessRegistrationStub concrete;
    const ir::Registration2D& base = concrete;

    const ir::RegistrationResult result = base.run(fixed, moving, transform);
    REQUIRE(result.succeeded);
}

TEST_CASE("Registration2D unique_ptr ownership", "[registration][Registration2D]") {
    const ir::Image2D fixed = makeImage(1, 1);
    const ir::Image2D moving = makeImage(1, 1);
    ir::Translation2D transform;
    auto method = std::make_unique<SuccessRegistrationStub>();

    const ir::RegistrationResult result = method->run(fixed, moving, transform);
    REQUIRE(result.succeeded);
    REQUIRE(result.iterations == 7);
}
