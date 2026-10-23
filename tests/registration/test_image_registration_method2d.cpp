#include <ir/core/ErrorCode.hpp>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/NearestNeighborInterpolator2D.hpp>
#include <ir/metric/MeanSquaresMetric2D.hpp>
#include <ir/optimizer/RegularStepGradientDescent.hpp>
#include <ir/registration/ImageRegistrationMethod2D.hpp>
#include <ir/registration/Registration2D.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <ir/resampler/ImageResampler2D.hpp>
#include <ir/transform/Transform2D.hpp>
#include <ir/transform/Translation2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

[[nodiscard]] ir::ImageGeometry2D makeGeom(int w, int h) {
    return ir::ImageGeometry2D{w, h, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
}

/** Disk of value 1.0; background 0. */
[[nodiscard]] ir::Image2D makeDisk(int w, int h, double cx, double cy, double radius) {
    ir::Image2D image{makeGeom(w, h)};
    const double r2 = radius * radius;
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            const double dx = static_cast<double>(i) - cx;
            const double dy = static_cast<double>(j) - cy;
            if (dx * dx + dy * dy <= r2) {
                image.at(i, j) = 1.0;
            }
        }
    }
    return image;
}

/** Non-parametric transform for failure-path test. */
class BareTransform final : public ir::Transform2D {
public:
    [[nodiscard]] ir::Point2D transformPoint(const ir::Point2D& p) const override { return p; }
    [[nodiscard]] bool isInvertible() const noexcept override { return true; }
};

} // namespace

TEST_CASE("ImageRegistrationMethod2D is a concrete Registration2D",
          "[registration][ImageRegistrationMethod2D]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Registration2D, ir::ImageRegistrationMethod2D>);
    STATIC_REQUIRE(std::is_final_v<ir::ImageRegistrationMethod2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::ImageRegistrationMethod2D>);
}

TEST_CASE("ImageRegistrationMethod2D rejects non-parametric transform",
          "[registration][ImageRegistrationMethod2D]") {
    const ir::Image2D fixed = makeDisk(16, 16, 8.0, 8.0, 3.0);
    const ir::Image2D moving = fixed;
    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::NearestNeighborInterpolator2D interpolator;
    const ir::RegularStepGradientDescent optimizer{10, 0.1};
    const ir::ImageRegistrationMethod2D method(metric, resampler, interpolator, optimizer);

    BareTransform transform;
    const ir::RegistrationResult r = method.run(fixed, moving, transform);
    REQUIRE_FALSE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::NotImplemented);
}

TEST_CASE("ImageRegistrationMethod2D recovers known Translation2D",
          "[registration][ImageRegistrationMethod2D]") {
    constexpr int kW = 48;
    constexpr int kH = 48;
    constexpr double kRadius = 8.0;
    constexpr double kShiftX = 5.0;
    constexpr double kShiftY = 0.0;

    const ir::Image2D fixed = makeDisk(kW, kH, 18.0, 24.0, kRadius);
    const ir::Image2D moving = makeDisk(kW, kH, 18.0 + kShiftX, 24.0 + kShiftY, kRadius);

    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::NearestNeighborInterpolator2D interpolator;
    // Larger step / iterations for discrete NN metric landscape
    const ir::RegularStepGradientDescent optimizer{80, 0.5, 1e-4, 1e-6, 2.0, 1.1, 0.5, 0.5};

    const ir::ImageRegistrationMethod2D method(metric, resampler, interpolator, optimizer);
    ir::Translation2D transform; // start at identity

    const double mseBefore = metric.evaluate(fixed, moving);
    const ir::RegistrationResult r = method.run(fixed, moving, transform);

    REQUIRE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Ok);
    REQUIRE(r.finalMetric <= mseBefore + 1e-9);

    REQUIRE_THAT(transform.offset().x(), WithinAbs(kShiftX, 1.5));
    REQUIRE_THAT(transform.offset().y(), WithinAbs(kShiftY, 1.5));
}

TEST_CASE("ImageRegistrationMethod2D works through Registration2D base",
          "[registration][ImageRegistrationMethod2D]") {
    const ir::Image2D fixed = makeDisk(32, 32, 12.0, 16.0, 6.0);
    const ir::Image2D moving = makeDisk(32, 32, 14.0, 16.0, 6.0);

    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::NearestNeighborInterpolator2D interpolator;
    const ir::RegularStepGradientDescent optimizer{40, 0.4, 1e-4, 1e-6, 2.0, 1.1, 0.5, 0.5};
    const ir::ImageRegistrationMethod2D concrete(metric, resampler, interpolator, optimizer);
    const ir::Registration2D& base = concrete;

    ir::Translation2D transform;
    const ir::RegistrationResult r = base.run(fixed, moving, transform);
    REQUIRE(r.succeeded);
}
