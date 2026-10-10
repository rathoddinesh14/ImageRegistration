#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/metric/Metric2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

[[nodiscard]] ir::Image2D makeConstantImage(double value) {
    ir::Image2D image{ir::ImageGeometry2D{2, 2, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}}};
    for (int j = 0; j < image.height(); ++j) {
        for (int i = 0; i < image.width(); ++i) {
            image.at(i, j) = value;
        }
    }
    return image;
}

/** Test-only: mean of (fixed - moving) over all pixels; minimize. */
class MeanDiffStub final : public ir::Metric2D {
public:
    [[nodiscard]] double evaluate(const ir::Image2D& fixed,
                                  const ir::Image2D& moving) const override {
        double sum = 0.0;
        int n = 0;
        for (int j = 0; j < fixed.height(); ++j) {
            for (int i = 0; i < fixed.width(); ++i) {
                sum += fixed.at(i, j) - moving.at(i, j);
                ++n;
            }
        }
        return n > 0 ? sum / static_cast<double>(n) : 0.0;
    }

    [[nodiscard]] bool isMinimize() const noexcept override { return true; }
};

/** Test-only similarity-style stub (higher is better). */
class ConstantSimilarityStub final : public ir::Metric2D {
public:
    [[nodiscard]] double evaluate(const ir::Image2D&, const ir::Image2D&) const override {
        return 1.0;
    }

    [[nodiscard]] bool isMinimize() const noexcept override { return false; }
};

} // namespace

TEST_CASE("Metric2D is an abstract polymorphic base", "[metric][Metric2D]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::Metric2D>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::Metric2D>);
}

TEST_CASE("Metric2D evaluate works via concrete stub", "[metric][Metric2D]") {
    const ir::Image2D fixed = makeConstantImage(5.0);
    const ir::Image2D moving = makeConstantImage(2.0);
    const MeanDiffStub metric;

    REQUIRE_THAT(metric.evaluate(fixed, moving), WithinAbs(3.0, kTol));
}

TEST_CASE("Metric2D works through base reference", "[metric][Metric2D]") {
    const ir::Image2D fixed = makeConstantImage(1.0);
    const ir::Image2D moving = makeConstantImage(1.0);
    const MeanDiffStub concrete;
    const ir::Metric2D& base = concrete;

    REQUIRE_THAT(base.evaluate(fixed, moving), WithinAbs(0.0, kTol));
    REQUIRE(base.isMinimize());
}

TEST_CASE("Metric2D isMinimize reports cost direction", "[metric][Metric2D]") {
    const MeanDiffStub cost;
    const ConstantSimilarityStub similarity;

    REQUIRE(cost.isMinimize());
    REQUIRE_FALSE(similarity.isMinimize());
}

TEST_CASE("Metric2D unique_ptr polymorphic ownership", "[metric][Metric2D]") {
    const ir::Image2D fixed = makeConstantImage(4.0);
    const ir::Image2D moving = makeConstantImage(1.0);
    std::unique_ptr<ir::Metric2D> metric = std::make_unique<MeanDiffStub>();

    REQUIRE_THAT(metric->evaluate(fixed, moving), WithinAbs(3.0, kTol));
    REQUIRE(metric->isMinimize());
}

TEST_CASE("Metric2D evaluate is const-callable", "[metric][Metric2D]") {
    const ir::Image2D fixed = makeConstantImage(0.0);
    const ir::Image2D moving = makeConstantImage(0.0);
    const MeanDiffStub metric;
    const double value = metric.evaluate(fixed, moving);
    REQUIRE_THAT(value, WithinAbs(0.0, kTol));
}
