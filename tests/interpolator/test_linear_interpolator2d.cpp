#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/BoundsPolicy.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/interpolator/LinearInterpolator2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

[[nodiscard]] ir::Image2D makeImage2x2() {
    ir::Image2D image{ir::ImageGeometry2D{2, 2, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}}};
    image.at(0, 0) = 1.0;
    image.at(1, 0) = 2.0;
    image.at(0, 1) = 3.0;
    image.at(1, 1) = 4.0;
    return image;
}

} // namespace

TEST_CASE("LinearInterpolator matches values at integer centers", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D lin;

    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{0.0, 0.0}), WithinAbs(1.0, kTol));
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{1.0, 0.0}), WithinAbs(2.0, kTol));
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{0.0, 1.0}), WithinAbs(3.0, kTol));
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{1.0, 1.0}), WithinAbs(4.0, kTol));
}

TEST_CASE("LinearInterpolator horizontal midpoint is average", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D lin;

    // Between (0,0)=1 and (1,0)=2
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{0.5, 0.0}), WithinAbs(1.5, kTol));
}

TEST_CASE("LinearInterpolator vertical midpoint is average", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D lin;

    // Between (0,0)=1 and (0,1)=3
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{0.0, 0.5}), WithinAbs(2.0, kTol));
}

TEST_CASE("LinearInterpolator cell center is bilinear blend", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D lin;

    // Center of unit square: average of 1,2,3,4 = 2.5
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{0.5, 0.5}), WithinAbs(2.5, kTol));
}

TEST_CASE("LinearInterpolator Constant OOB uses fill for missing corners",
          "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D lin{ir::BoundsPolicy::Constant, 0.0};

    // Far outside: all corners OOB -> 0
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{-5.0, -5.0}), WithinAbs(0.0, kTol));
}

TEST_CASE("LinearInterpolator Clamp OOB uses edge values", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D lin{ir::BoundsPolicy::Clamp};

    // Left of image along j=0 -> edge value 1
    REQUIRE_THAT(lin.evaluate(image, ir::Point2D{-1.0, 0.0}), WithinAbs(1.0, kTol));
}

TEST_CASE("LinearInterpolator is concrete Interpolator2D", "[interpolator][Linear]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Interpolator2D, ir::LinearInterpolator2D>);
    STATIC_REQUIRE(std::is_final_v<ir::LinearInterpolator2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::LinearInterpolator2D>);
}

TEST_CASE("LinearInterpolator works through Interpolator2D base", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    const ir::LinearInterpolator2D concrete;
    const ir::Interpolator2D& base = concrete;

    REQUIRE_THAT(base.evaluate(image, ir::Point2D{0.5, 0.5}), WithinAbs(2.5, kTol));
}

TEST_CASE("LinearInterpolator unique_ptr ownership", "[interpolator][Linear]") {
    const ir::Image2D image = makeImage2x2();
    auto interpolator = std::make_unique<ir::LinearInterpolator2D>();

    REQUIRE_THAT(interpolator->evaluate(image, ir::Point2D{0.0, 0.0}), WithinAbs(1.0, kTol));
}
