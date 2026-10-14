/**
 * Synthetic translation demo using existing ImageRegistration types.
 *
 * Builds a fixed image with a bright disk, a moving image whose disk is
 * shifted by a known integer offset, then:
 *   1. Computes MSE between fixed and (misaligned) moving
 *   2. Resamples moving onto fixed geometry with Translation2D + NN
 *   3. Computes MSE after resampling (should drop near zero)
 *   4. Optionally writes PNGs when built with IR_BUILD_IO
 *
 * Configure:
 *   cmake -S . -B build -DIR_BUILD_EXAMPLES=ON -DIR_BUILD_IO=ON
 * Run (from build dir):
 *   ./examples/synthetic_translation/ir_example_synthetic_translation [out_dir]
 */

#include <cmath>
#include <iostream>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/NearestNeighborInterpolator2D.hpp>
#include <ir/metric/MeanSquaresMetric2D.hpp>
#include <ir/resampler/ImageResampler2D.hpp>
#include <ir/transform/Translation2D.hpp>
#include <string>

#if defined(IR_HAS_IO)
#include <ir/io/ImageWrite.hpp>
#endif

namespace {

[[nodiscard]] ir::ImageGeometry2D makeGeom(int w, int h) {
    return ir::ImageGeometry2D{w, h, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
}

/** Disk of value 1.0 centered near (cx, cy) in index space; background 0. */
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

} // namespace

int main(int argc, char** argv) {
    const std::string outDir = (argc > 1) ? argv[1] : ".";

    constexpr int kW = 64;
    constexpr int kH = 64;
    constexpr double kRadius = 10.0;
    constexpr int kShiftI = 8; // moving disk is 8 pixels to the right of fixed

    const ir::Image2D fixed = makeDisk(kW, kH, 24.0, 32.0, kRadius);
    const ir::Image2D moving = makeDisk(kW, kH, 24.0 + kShiftI, 32.0, kRadius);

    const ir::MeanSquaresMetric2D metric;
    const double mseBefore = metric.evaluate(fixed, moving);

    // fixed physical → moving physical: +shift in i (unit spacing)
    const ir::Translation2D transform{ir::Point2D{static_cast<double>(kShiftI), 0.0}};
    const ir::NearestNeighborInterpolator2D interpolator;
    const ir::ImageResampler2D resampler;

    const ir::Image2D aligned =
        resampler.resample(moving, fixed.geometry(), transform, interpolator);
    const double mseAfter = metric.evaluate(fixed, aligned);

    std::cout << "Synthetic translation demo\n"
              << "  image size: " << kW << " x " << kH << "\n"
              << "  known shift (index i): +" << kShiftI << "\n"
              << "  MSE before resample: " << mseBefore << "\n"
              << "  MSE after  resample: " << mseAfter << "\n";

    if (!(mseAfter < mseBefore)) {
        std::cerr << "Unexpected: MSE did not decrease after alignment.\n";
        return 1;
    }
    if (mseAfter > 1e-12) {
        std::cerr << "Warning: residual MSE not exactly zero (NN + integer shift expected 0).\n";
    }

#if defined(IR_HAS_IO)
    const std::string fixedPath = outDir + "/fixed.png";
    const std::string movingPath = outDir + "/moving.png";
    const std::string alignedPath = outDir + "/aligned.png";
    const bool okFixed = ir::io::write(fixed, fixedPath);
    const bool okMoving = ir::io::write(moving, movingPath);
    const bool okAligned = ir::io::write(aligned, alignedPath);
    std::cout << "  wrote PNGs: " << (okFixed && okMoving && okAligned ? "yes" : "partial/fail")
              << " under " << outDir << "\n";
#else
    std::cout << "  PNG export skipped (build with -DIR_BUILD_IO=ON and IR_HAS_IO).\n";
    (void)outDir;
#endif

    return 0;
}
