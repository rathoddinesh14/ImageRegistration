/**
 * End-to-end registration demo: recover a known 2D translation.
 *
 * Builds smooth Gaussian blobs (fixed + shifted moving), then runs
 * ImageRegistrationMethod2D with mean-squares, linear interpolation,
 * ImageResampler2D, and RegularStepGradientDescent.
 *
 * Configure:
 *   cmake -S . -B build -DIR_BUILD_EXAMPLES=ON
 * Run (from build dir):
 *   ./examples/register_translation/ir_example_register_translation
 */

#include <cmath>
#include <iostream>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/LinearInterpolator2D.hpp>
#include <ir/metric/MeanSquaresMetric2D.hpp>
#include <ir/optimizer/RegularStepGradientDescent.hpp>
#include <ir/registration/ImageRegistrationMethod2D.hpp>
#include <ir/registration/RegistrationResult.hpp>
#include <ir/resampler/ImageResampler2D.hpp>
#include <ir/transform/Translation2D.hpp>

namespace {

[[nodiscard]] ir::ImageGeometry2D makeGeom(int w, int h) {
    return ir::ImageGeometry2D{w, h, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
}

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

} // namespace

int main() {
    constexpr int kW = 48;
    constexpr int kH = 48;
    constexpr double kSigma = 3.5;
    constexpr double kTrueTx = 4.0;
    constexpr double kTrueTy = 1.0;

    const ir::Image2D fixed = makeBlob(kW, kH, 18.0, 22.0, kSigma);
    const ir::Image2D moving = makeBlob(kW, kH, 18.0 + kTrueTx, 22.0 + kTrueTy, kSigma);

    const ir::MeanSquaresMetric2D metric;
    const ir::ImageResampler2D resampler;
    const ir::LinearInterpolator2D interpolator;
    const ir::RegularStepGradientDescent optimizer{150, 0.25, 1e-5, 1e-8, 1.0, 1.05, 0.5, 0.4};
    const ir::ImageRegistrationMethod2D method(metric, resampler, interpolator, optimizer);

    ir::Translation2D transform; // identity start
    const double mseBefore = metric.evaluate(fixed, moving);
    const ir::RegistrationResult result = method.run(fixed, moving, transform);

    std::cout << "ImageRegistration end-to-end translation recovery\n";
    std::cout << "  true offset:     (" << kTrueTx << ", " << kTrueTy << ")\n";
    std::cout << "  MSE before:      " << mseBefore << "\n";
    std::cout << "  succeeded:       " << (result.succeeded ? "yes" : "no") << "\n";
    std::cout << "  final metric:    " << result.finalMetric << "\n";
    std::cout << "  iterations:      " << result.iterations << "\n";
    std::cout << "  recovered offset:(" << transform.offset().x() << ", " << transform.offset().y()
              << ")\n";

    if (!result.succeeded) {
        std::cerr << "Registration failed: " << result.message << "\n";
        return 1;
    }
    return 0;
}
