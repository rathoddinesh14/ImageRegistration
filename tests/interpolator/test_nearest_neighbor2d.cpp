#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/BoundsPolicy.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/interpolator/NearestNeighborInterpolator2D.hpp>
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

TEST_CASE("NearestNeighbor samples exact integer centers", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    const ir::NearestNeighborInterpolator2D nn;

    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{0.0, 0.0}), WithinAbs(1.0, kTol));
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{1.0, 0.0}), WithinAbs(2.0, kTol));
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{0.0, 1.0}), WithinAbs(3.0, kTol));
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{1.0, 1.0}), WithinAbs(4.0, kTol));
}

TEST_CASE("NearestNeighbor rounds to nearest lattice point", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    const ir::NearestNeighborInterpolator2D nn;

    // 0.4 -> 0, 0.6 -> 1
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{0.4, 0.0}), WithinAbs(1.0, kTol));
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{0.6, 0.0}), WithinAbs(2.0, kTol));
}

TEST_CASE("NearestNeighbor Constant OOB returns fill value", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    const ir::NearestNeighborInterpolator2D nn{ir::BoundsPolicy::Constant, -7.0};

    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{-1.0, 0.0}), WithinAbs(-7.0, kTol));
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{0.0, 5.0}), WithinAbs(-7.0, kTol));
}

TEST_CASE("NearestNeighbor default Constant fill is 0", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    const ir::NearestNeighborInterpolator2D nn;

    REQUIRE(nn.boundsPolicy() == ir::BoundsPolicy::Constant);
    REQUIRE_THAT(nn.fillValue(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{-1.0, -1.0}), WithinAbs(0.0, kTol));
}

TEST_CASE("NearestNeighbor Clamp uses edge pixels", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    const ir::NearestNeighborInterpolator2D nn{ir::BoundsPolicy::Clamp};

    // Left of (0,0) -> (0,0) = 1
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{-2.0, 0.0}), WithinAbs(1.0, kTol));
    // Right of (1,0) -> (1,0) = 2
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{5.0, 0.0}), WithinAbs(2.0, kTol));
    // Below (0,1) -> (0,1) = 3
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{0.0, 8.0}), WithinAbs(3.0, kTol));
    // Corner beyond (1,1) -> (1,1) = 4
    REQUIRE_THAT(nn.evaluate(image, ir::Point2D{9.0, 9.0}), WithinAbs(4.0, kTol));
}

TEST_CASE("NearestNeighbor is concrete Interpolator2D", "[interpolator][NearestNeighbor]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Interpolator2D, ir::NearestNeighborInterpolator2D>);
    STATIC_REQUIRE(std::is_final_v<ir::NearestNeighborInterpolator2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::NearestNeighborInterpolator2D>);
}

TEST_CASE("NearestNeighbor works through Interpolator2D base", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    const ir::NearestNeighborInterpolator2D concrete;
    const ir::Interpolator2D& base = concrete;

    REQUIRE_THAT(base.evaluate(image, ir::Point2D{1.0, 1.0}), WithinAbs(4.0, kTol));
}

TEST_CASE("NearestNeighbor unique_ptr ownership", "[interpolator][NearestNeighbor]") {
    const ir::Image2D image = makeImage2x2();
    std::unique_ptr<ir::Interpolator2D> interpolator =
        std::make_unique<ir::NearestNeighborInterpolator2D>();

    REQUIRE_THAT(interpolator->evaluate(image, ir::Point2D{0.0, 0.0}), WithinAbs(1.0, kTol));
}
