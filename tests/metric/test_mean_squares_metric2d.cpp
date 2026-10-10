#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/metric/MeanSquaresMetric2D.hpp>
#include <ir/metric/Metric2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

[[nodiscard]] ir::Image2D makeConstantImage(int w, int h, double value) {
    ir::Image2D image{ir::ImageGeometry2D{w, h, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}}};
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            image.at(i, j) = value;
        }
    }
    return image;
}

} // namespace

TEST_CASE("MeanSquares identical images yield zero", "[metric][MeanSquares]") {
    const ir::Image2D a = makeConstantImage(3, 2, 1.5);
    const ir::Image2D b = makeConstantImage(3, 2, 1.5);
    const ir::MeanSquaresMetric2D metric;

    REQUIRE_THAT(metric.evaluate(a, b), WithinAbs(0.0, kTol));
}

TEST_CASE("MeanSquares constant difference is squared", "[metric][MeanSquares]") {
    const ir::Image2D fixed = makeConstantImage(2, 2, 5.0);
    const ir::Image2D moving = makeConstantImage(2, 2, 2.0);
    const ir::MeanSquaresMetric2D metric;

    // (5-2)^2 = 9 for every pixel
    REQUIRE_THAT(metric.evaluate(fixed, moving), WithinAbs(9.0, kTol));
}

TEST_CASE("MeanSquares is always minimize", "[metric][MeanSquares]") {
    const ir::MeanSquaresMetric2D metric;
    REQUIRE(metric.isMinimize());
}

TEST_CASE("MeanSquares is final concrete Metric2D", "[metric][MeanSquares]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Metric2D, ir::MeanSquaresMetric2D>);
    STATIC_REQUIRE(std::is_final_v<ir::MeanSquaresMetric2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::MeanSquaresMetric2D>);
}

TEST_CASE("MeanSquares works through Metric2D base", "[metric][MeanSquares]") {
    const ir::Image2D fixed = makeConstantImage(2, 2, 1.0);
    const ir::Image2D moving = makeConstantImage(2, 2, 0.0);
    const ir::MeanSquaresMetric2D concrete;
    const ir::Metric2D& base = concrete;

    REQUIRE_THAT(base.evaluate(fixed, moving), WithinAbs(1.0, kTol));
    REQUIRE(base.isMinimize());
}

TEST_CASE("MeanSquares unique_ptr ownership", "[metric][MeanSquares]") {
    const ir::Image2D fixed = makeConstantImage(1, 1, 4.0);
    const ir::Image2D moving = makeConstantImage(1, 1, 1.0);
    auto metric = std::make_unique<ir::MeanSquaresMetric2D>();

    REQUIRE_THAT(metric->evaluate(fixed, moving), WithinAbs(9.0, kTol));
}

TEST_CASE("MeanSquares single differing pixel", "[metric][MeanSquares]") {
    ir::Image2D fixed = makeConstantImage(2, 2, 0.0);
    ir::Image2D moving = makeConstantImage(2, 2, 0.0);
    fixed.at(1, 1) = 2.0;
    // MSE = (2^2) / 4 = 1
    const ir::MeanSquaresMetric2D metric;
    REQUIRE_THAT(metric.evaluate(fixed, moving), WithinAbs(1.0, kTol));
}
