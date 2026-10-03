#include <cmath>
#include <ir/core/Point2D.hpp>
#include <ir/transform/Transform2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

/**
 * Test-only stub: translation by a fixed offset.
 * Lives in the test TU only; not a production transform.
 */
class TranslationStub final : public ir::Transform2D {
public:
    explicit TranslationStub(ir::Point2D offset) noexcept : m_offset(offset) {}

    [[nodiscard]] ir::Point2D transformPoint(const ir::Point2D& p) const override {
        return p + m_offset;
    }

    [[nodiscard]] bool isInvertible() const noexcept override { return true; }

private:
    ir::Point2D m_offset;
};

/** Non-invertible stub for capability checks. */
class NonInvertibleStub final : public ir::Transform2D {
public:
    [[nodiscard]] ir::Point2D transformPoint(const ir::Point2D& p) const override { return p; }

    [[nodiscard]] bool isInvertible() const noexcept override { return false; }
};

} // namespace

TEST_CASE("Transform2D is an abstract polymorphic base", "[transform][Transform2D]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::Transform2D>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::Transform2D>);
}

TEST_CASE("Transform2D transformPoint maps via concrete impl", "[transform][Transform2D]") {
    const TranslationStub transform{ir::Point2D{2.0, -3.0}};
    const ir::Point2D input{1.0, 4.0};

    const ir::Point2D out = transform.transformPoint(input);

    REQUIRE_THAT(out.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(1.0, kTol));
}

TEST_CASE("Transform2D works through base reference and pointer", "[transform][Transform2D]") {
    const TranslationStub concrete{ir::Point2D{1.0, 1.0}};
    const ir::Transform2D& asRef = concrete;
    const ir::Transform2D* asPtr = &concrete;

    const ir::Point2D p{5.0, 7.0};
    const ir::Point2D fromRef = asRef.transformPoint(p);
    const ir::Point2D fromPtr = asPtr->transformPoint(p);

    REQUIRE_THAT(fromRef.x(), WithinAbs(6.0, kTol));
    REQUIRE_THAT(fromRef.y(), WithinAbs(8.0, kTol));
    REQUIRE_THAT(fromPtr.x(), WithinAbs(6.0, kTol));
    REQUIRE_THAT(fromPtr.y(), WithinAbs(8.0, kTol));
}

TEST_CASE("Transform2D transformPoint is const-callable", "[transform][Transform2D]") {
    const TranslationStub transform{ir::Point2D{0.5, 0.25}};
    const ir::Point2D out = transform.transformPoint(ir::Point2D{0.0, 0.0});

    REQUIRE_THAT(out.x(), WithinAbs(0.5, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(0.25, kTol));
}

TEST_CASE("Transform2D zero offset leaves origin fixed", "[transform][Transform2D]") {
    const TranslationStub identity{ir::Point2D{0.0, 0.0}};
    const ir::Point2D origin{};
    const ir::Point2D out = identity.transformPoint(origin);

    REQUIRE_THAT(out.x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(0.0, kTol));
}

TEST_CASE("Transform2D isInvertible true for invertible stub", "[transform][Transform2D]") {
    const TranslationStub transform{ir::Point2D{1.0, 0.0}};
    REQUIRE(transform.isInvertible());
    REQUIRE(static_cast<const ir::Transform2D&>(transform).isInvertible());
}

TEST_CASE("Transform2D isInvertible false for non-invertible stub", "[transform][Transform2D]") {
    const NonInvertibleStub transform;
    REQUIRE_FALSE(transform.isInvertible());
}

TEST_CASE("Transform2D unique_ptr polymorphic ownership", "[transform][Transform2D]") {
    std::unique_ptr<ir::Transform2D> transform =
        std::make_unique<TranslationStub>(ir::Point2D{-1.0, 2.0});

    const ir::Point2D out = transform->transformPoint(ir::Point2D{3.0, 3.0});
    REQUIRE_THAT(out.x(), WithinAbs(2.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(5.0, kTol));
    REQUIRE(transform->isInvertible());
}

TEST_CASE("Transform2D inverse API deferred to Expected", "[transform][Transform2D]") {
    // Invertibility is discoverable; materializing inverse waits for ir::Expected.
    const TranslationStub invertible{ir::Point2D{1.0, 1.0}};
    const NonInvertibleStub notInvertible{};

    REQUIRE(invertible.isInvertible());
    REQUIRE_FALSE(notInvertible.isInvertible());
}
