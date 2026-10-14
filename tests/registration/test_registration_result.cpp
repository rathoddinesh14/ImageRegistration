#include <ir/core/ErrorCode.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <string>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

} // namespace

TEST_CASE("RegistrationResult default is unsuccessful Ok-shaped zeros",
          "[registration][RegistrationResult]") {
    const ir::RegistrationResult r;
    REQUIRE_FALSE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Ok);
    REQUIRE_THAT(r.finalMetric, WithinAbs(0.0, kTol));
    REQUIRE(r.iterations == 0);
    REQUIRE(r.message.empty());
}

TEST_CASE("RegistrationResult success factory", "[registration][RegistrationResult]") {
    const ir::RegistrationResult r = ir::RegistrationResult::success(1.25, 42);
    REQUIRE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Ok);
    REQUIRE_THAT(r.finalMetric, WithinAbs(1.25, kTol));
    REQUIRE(r.iterations == 42);
    REQUIRE(r.message.empty());
}

TEST_CASE("RegistrationResult failure factory", "[registration][RegistrationResult]") {
    const ir::RegistrationResult r =
        ir::RegistrationResult::failure(ir::ErrorCode::EmptyOverlap, "no overlap");
    REQUIRE_FALSE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::EmptyOverlap);
    REQUIRE(r.message == "no overlap");
}

TEST_CASE("RegistrationResult is a regular value type", "[registration][RegistrationResult]") {
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<ir::RegistrationResult>);
    STATIC_REQUIRE(std::is_copy_constructible_v<ir::RegistrationResult>);
    STATIC_REQUIRE(std::is_move_constructible_v<ir::RegistrationResult>);
}

TEST_CASE("ErrorCode enumerators exist for registration diagnostics",
          "[registration][ErrorCode]") {
    REQUIRE(static_cast<int>(ir::ErrorCode::Ok) == 0);
    REQUIRE(ir::ErrorCode::SingularTransform != ir::ErrorCode::Ok);
    REQUIRE(ir::ErrorCode::NonFiniteMetric != ir::ErrorCode::Unknown);
}
