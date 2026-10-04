#include <cmath>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
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

/**
 * Test-only stub: returns the nearest integer sample, or 0.0 if OOB.
 */
class NearestStub final : public ir::Interpolator2D {
public:
    [[nodiscard]] double evaluate(const ir::Image2D& image,
                                  const ir::Point2D& continuousIndex) const override {
        const int i = static_cast<int>(std::lround(continuousIndex.x()));
        const int j = static_cast<int>(std::lround(continuousIndex.y()));
        if (!image.containsIndex(i, j)) {
            return 0.0;
        }
        return image.at(i, j);
    }
};

} // namespace

TEST_CASE("Interpolator2D is an abstract polymorphic base", "[interpolator][Interpolator2D]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::Interpolator2D>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::Interpolator2D>);
}

TEST_CASE("Interpolator2D evaluate samples via concrete stub", "[interpolator][Interpolator2D]") {
    const ir::Image2D image = makeImage2x2();
    const NearestStub interpolator;

    REQUIRE_THAT(interpolator.evaluate(image, ir::Point2D{0.0, 0.0}), WithinAbs(1.0, kTol));
    REQUIRE_THAT(interpolator.evaluate(image, ir::Point2D{1.0, 0.0}), WithinAbs(2.0, kTol));
    REQUIRE_THAT(interpolator.evaluate(image, ir::Point2D{0.0, 1.0}), WithinAbs(3.0, kTol));
    REQUIRE_THAT(interpolator.evaluate(image, ir::Point2D{1.0, 1.0}), WithinAbs(4.0, kTol));
}

TEST_CASE("Interpolator2D works through base reference", "[interpolator][Interpolator2D]") {
    const ir::Image2D image = makeImage2x2();
    const NearestStub concrete;
    const ir::Interpolator2D& base = concrete;

    REQUIRE_THAT(base.evaluate(image, ir::Point2D{1.0, 1.0}), WithinAbs(4.0, kTol));
}

TEST_CASE("Interpolator2D stub returns 0 outside bounds", "[interpolator][Interpolator2D]") {
    const ir::Image2D image = makeImage2x2();
    const NearestStub interpolator;

    REQUIRE_THAT(interpolator.evaluate(image, ir::Point2D{-1.0, 0.0}), WithinAbs(0.0, kTol));
    REQUIRE_THAT(interpolator.evaluate(image, ir::Point2D{0.0, 5.0}), WithinAbs(0.0, kTol));
}

TEST_CASE("Interpolator2D unique_ptr polymorphic ownership", "[interpolator][Interpolator2D]") {
    const ir::Image2D image = makeImage2x2();
    std::unique_ptr<ir::Interpolator2D> interpolator = std::make_unique<NearestStub>();

    REQUIRE_THAT(interpolator->evaluate(image, ir::Point2D{0.0, 0.0}), WithinAbs(1.0, kTol));
}

TEST_CASE("Interpolator2D evaluate is const-callable", "[interpolator][Interpolator2D]") {
    const ir::Image2D image = makeImage2x2();
    const NearestStub interpolator;
    const double value = interpolator.evaluate(image, ir::Point2D{0.0, 0.0});
    REQUIRE_THAT(value, WithinAbs(1.0, kTol));
}
