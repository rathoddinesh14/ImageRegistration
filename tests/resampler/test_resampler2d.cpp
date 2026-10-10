#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/interpolator/Interpolator2D.hpp>
#include <ir/resampler/Resampler2D.hpp>
#include <ir/transform/Transform2D.hpp>
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

/** Test-only: output is zero-filled image with fixed geometry (ignores inputs). */
class ZeroResamplerStub final : public ir::Resampler2D {
public:
    [[nodiscard]] ir::Image2D resample(const ir::Image2D& /*moving*/,
                                       const ir::ImageGeometry2D& fixedGeometry,
                                       const ir::Transform2D& /*transform*/,
                                       const ir::Interpolator2D& /*interpolator*/) const override {
        return ir::Image2D{fixedGeometry};
    }
};

/** Test-only interpolator: always returns a constant. */
class ConstInterpolatorStub final : public ir::Interpolator2D {
public:
    explicit ConstInterpolatorStub(double v) noexcept : m_value(v) {}

    [[nodiscard]] double evaluate(const ir::Image2D&, const ir::Point2D&) const override {
        return m_value;
    }

private:
    double m_value = 0.0;
};

} // namespace

TEST_CASE("Resampler2D is an abstract polymorphic base", "[resampler][Resampler2D]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::Resampler2D>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::Resampler2D>);
}

TEST_CASE("Resampler2D stub returns image with fixed geometry", "[resampler][Resampler2D]") {
    const ir::Image2D moving{unitGeom(2, 2)};
    const ir::ImageGeometry2D fixed = unitGeom(3, 1);
    const ir::Translation2D transform{ir::Point2D{0.0, 0.0}};
    const ConstInterpolatorStub interpolator{1.0};
    const ZeroResamplerStub resampler;

    const ir::Image2D out = resampler.resample(moving, fixed, transform, interpolator);

    REQUIRE(out.width() == 3);
    REQUIRE(out.height() == 1);
    REQUIRE_THAT(out.at(0, 0), WithinAbs(0.0, kTol));
}

TEST_CASE("Resampler2D works through base reference", "[resampler][Resampler2D]") {
    const ir::Image2D moving{unitGeom(2, 2)};
    const ir::ImageGeometry2D fixed = unitGeom(2, 2);
    const ir::Translation2D transform;
    const ConstInterpolatorStub interpolator{0.0};
    const ZeroResamplerStub concrete;
    const ir::Resampler2D& base = concrete;

    const ir::Image2D out = base.resample(moving, fixed, transform, interpolator);
    REQUIRE(out.width() == 2);
    REQUIRE(out.height() == 2);
}

TEST_CASE("Resampler2D unique_ptr polymorphic ownership", "[resampler][Resampler2D]") {
    const ir::Image2D moving{unitGeom(1, 1)};
    const ir::ImageGeometry2D fixed = unitGeom(4, 5);
    const ir::Translation2D transform;
    const ConstInterpolatorStub interpolator{0.0};
    std::unique_ptr<ir::Resampler2D> resampler = std::make_unique<ZeroResamplerStub>();

    const ir::Image2D out = resampler->resample(moving, fixed, transform, interpolator);
    REQUIRE(out.width() == 4);
    REQUIRE(out.height() == 5);
}

TEST_CASE("Resampler2D resample is const-callable", "[resampler][Resampler2D]") {
    const ir::Image2D moving{unitGeom(2, 2)};
    const ir::ImageGeometry2D fixed = unitGeom(2, 2);
    const ir::Translation2D transform;
    const ConstInterpolatorStub interpolator{0.0};
    const ZeroResamplerStub resampler;

    const ir::Image2D out = resampler.resample(moving, fixed, transform, interpolator);
    REQUIRE(out.geometry() == fixed);
}
