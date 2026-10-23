#include <cmath>
#include <ir/core/ErrorCode.hpp>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/LinearInterpolator2D.hpp>
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

/** Smooth Gaussian blob (differentiable enough for finite-difference GD). */
[[nodiscard]] ir::Image2D makeBlob(int w, int h, double cx, double cy, double sigma) {
    ir::Image2D image{makeGeom(w, h)};
    const double inv = 1.0 / (2.0 * sigma * sigma);
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            const double dx = static_cast<double>(i) - cx;
            const double dy = static_cast<double>(j) - cy;
            image.at(i, j) = std::exp(-(dx * dx + dy * dy) * inv);
        }
    }
    return image;
}

class BareTransform final : public ir::Transform2D {
public:
    [[nodiscard]] ir::Point2D transformPoint(const ir::Point2D& p) const override { return p; }
    [[nodiscard]] bool isInvertible() const noexcept override { return true; }
};

} // namespace

TEST_CASE("ImageRegistrationMethod2D is concrete Registration2D",
          "[registration][ImageRegistrationMethod2D]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Registration2D, ir::ImageRegistrationMethod2D>);
    STATIC_REQUIRE(std::is_final_v<ir::ImageRegistrationMethod2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::ImageRegistrationMethod2D>);
}

TEST_CASE("ImageRegistrationMethod2D rejects non-parametric transform",
          "[registration][ImageRegistrationMethod2D]") {
    const ir::Image2D fixed = makeBlob(16, 16, 8.0, 8.0, 2.0);
    const ir::Image2D moving = fixed;
    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::LinearInterpolator2D interpolator;
    const ir::RegularStepGradientDescent optimizer{10, 0.1};
    const ir::ImageRegistrationMethod2D method(metric, resampler, interpolator, optimizer);

    BareTransform transform;
    const ir::RegistrationResult r = method.run(fixed, moving, transform);
    REQUIRE_FALSE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::NotImplemented);
}

TEST_CASE("ImageRegistrationMethod2D recovers known Translation2D",
          "[registration][ImageRegistrationMethod2D]") {
    // Small 1D shift + smooth blob so finite-diff regular-step GD can finish under CI.
    constexpr int kW = 32;
    constexpr int kH = 32;
    constexpr double kSigma = 4.0;
    constexpr double kShiftX = 1.5;
    constexpr double kShiftY = 0.0;

    const ir::Image2D fixed = makeBlob(kW, kH, 14.0, 16.0, kSigma);
    const ir::Image2D moving = makeBlob(kW, kH, 14.0 + kShiftX, 16.0 + kShiftY, kSigma);

    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::LinearInterpolator2D interpolator;
    // Image metrics need larger step scale / FD epsilon than analytic quadratics.
    const ir::RegularStepGradientDescent optimizer{250, 2.0, 1e-6, 1e-10, 4.0, 1.2, 0.5, 0.5};

    const ir::ImageRegistrationMethod2D method(metric, resampler, interpolator, optimizer);
    ir::Translation2D transform;

    const double mseBefore = metric.evaluate(fixed, moving);
    const ir::RegistrationResult r = method.run(fixed, moving, transform);

    REQUIRE(r.succeeded);
    REQUIRE(r.code == ir::ErrorCode::Ok);
    REQUIRE(r.finalMetric < mseBefore);
    REQUIRE_THAT(transform.offset().x(), WithinAbs(kShiftX, 0.75));
    REQUIRE_THAT(transform.offset().y(), WithinAbs(kShiftY, 0.75));
}

TEST_CASE("ImageRegistrationMethod2D works through Registration2D base",
          "[registration][ImageRegistrationMethod2D]") {
    const ir::Image2D fixed = makeBlob(32, 32, 12.0, 16.0, 2.5);
    const ir::Image2D moving = makeBlob(32, 32, 13.0, 16.0, 2.5);

    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::LinearInterpolator2D interpolator;
    const ir::RegularStepGradientDescent optimizer{60, 0.2, 1e-5, 1e-8, 1.0, 1.05, 0.5, 0.35};
    const ir::ImageRegistrationMethod2D concrete(metric, resampler, interpolator, optimizer);
    const ir::Registration2D& base = concrete;

    ir::Translation2D transform;
    const ir::RegistrationResult r = base.run(fixed, moving, transform);
    REQUIRE(r.succeeded);
}
