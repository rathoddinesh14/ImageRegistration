#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/NearestNeighborInterpolator2D.hpp>
#include <ir/resampler/ImageResampler2D.hpp>
#include <ir/resampler/Resampler2D.hpp>
#include <ir/transform/Translation2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

[[nodiscard]] ir::ImageGeometry2D unitGeom(int w, int h) {
    return ir::ImageGeometry2D{w, h, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
}

} // namespace

TEST_CASE("ImageResampler2D identity keeps values with NN", "[resampler][ImageResampler2D]") {
    ir::Image2D moving{unitGeom(2, 2)};
    moving.at(0, 0) = 1.0;
    moving.at(1, 0) = 2.0;
    moving.at(0, 1) = 3.0;
    moving.at(1, 1) = 4.0;

    const ir::Translation2D identity;
    const ir::NearestNeighborInterpolator2D nn;
    const ir::ImageResampler2D resampler;

    const ir::Image2D out = resampler.resample(moving, moving.geometry(), identity, nn);

    REQUIRE_THAT(out.at(0, 0), WithinAbs(1.0, kTol));
    REQUIRE_THAT(out.at(1, 0), WithinAbs(2.0, kTol));
    REQUIRE_THAT(out.at(0, 1), WithinAbs(3.0, kTol));
    REQUIRE_THAT(out.at(1, 1), WithinAbs(4.0, kTol));
}

TEST_CASE("ImageResampler2D integer translation shifts with NN", "[resampler][ImageResampler2D]") {
    // Moving: [10, 20, 30] on a 3x1 lattice (unit spacing, origin at pixel-center 0).
    ir::Image2D moving{unitGeom(3, 1)};
    moving.at(0, 0) = 10.0;
    moving.at(1, 0) = 20.0;
    moving.at(2, 0) = 30.0;

    // fixed → moving: p_m = p_f + (-1, 0) → fixed i samples moving i-1
    const ir::Translation2D transform{ir::Point2D{-1.0, 0.0}};
    const ir::NearestNeighborInterpolator2D nn;
    const ir::ImageResampler2D resampler;

    const ir::Image2D out = resampler.resample(moving, unitGeom(3, 1), transform, nn);

    // i=0 → moving index -1 → OOB Constant(0)
    REQUIRE_THAT(out.at(0, 0), WithinAbs(0.0, kTol));
    REQUIRE_THAT(out.at(1, 0), WithinAbs(10.0, kTol));
    REQUIRE_THAT(out.at(2, 0), WithinAbs(20.0, kTol));
}

TEST_CASE("ImageResampler2D is final concrete Resampler2D", "[resampler][ImageResampler2D]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Resampler2D, ir::ImageResampler2D>);
    STATIC_REQUIRE(std::is_final_v<ir::ImageResampler2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::ImageResampler2D>);
}

TEST_CASE("ImageResampler2D works through Resampler2D base", "[resampler][ImageResampler2D]") {
    ir::Image2D moving{unitGeom(2, 1)};
    moving.at(0, 0) = 5.0;
    moving.at(1, 0) = 7.0;

    const ir::Translation2D identity;
    const ir::NearestNeighborInterpolator2D nn;
    const ir::ImageResampler2D concrete;
    const ir::Resampler2D& base = concrete;

    const ir::Image2D out = base.resample(moving, unitGeom(2, 1), identity, nn);
    REQUIRE_THAT(out.at(0, 0), WithinAbs(5.0, kTol));
    REQUIRE_THAT(out.at(1, 0), WithinAbs(7.0, kTol));
}

TEST_CASE("ImageResampler2D unique_ptr ownership", "[resampler][ImageResampler2D]") {
    ir::Image2D moving{unitGeom(1, 1)};
    moving.at(0, 0) = 9.0;

    const ir::Translation2D identity;
    const ir::NearestNeighborInterpolator2D nn;
    auto resampler = std::make_unique<ir::ImageResampler2D>();

    const ir::Image2D out = resampler->resample(moving, unitGeom(1, 1), identity, nn);
    REQUIRE_THAT(out.at(0, 0), WithinAbs(9.0, kTol));
}

TEST_CASE("ImageResampler2D output uses fixed geometry size", "[resampler][ImageResampler2D]") {
    const ir::Image2D moving{unitGeom(2, 2)};
    const ir::ImageGeometry2D fixed = unitGeom(4, 3);
    const ir::Translation2D identity;
    const ir::NearestNeighborInterpolator2D nn;
    const ir::ImageResampler2D resampler;

    const ir::Image2D out = resampler.resample(moving, fixed, identity, nn);
    REQUIRE(out.width() == 4);
    REQUIRE(out.height() == 3);
}
