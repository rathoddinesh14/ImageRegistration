#include <ir/core/Point2D.hpp>
#include <ir/transform/Transform2D.hpp>
#include <ir/transform/Translation2D.hpp>
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

} // namespace

TEST_CASE("Translation2D default constructs to zero offset", "[transform][Translation2D]") {
    const ir::Translation2D transform;
    REQUIRE_THAT(transform.offset().x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(transform.offset().y(), WithinAbs(0.0, kTol));
}

TEST_CASE("Translation2D stores the constructed offset", "[transform][Translation2D]") {
    const ir::Translation2D transform{ir::Point2D{1.5, -2.5}};
    REQUIRE_THAT(transform.offset().x(), WithinAbs(1.5, kTol));
    REQUIRE_THAT(transform.offset().y(), WithinAbs(-2.5, kTol));
}

TEST_CASE("Translation2D transformPoint adds the offset", "[transform][Translation2D]") {
    const ir::Translation2D transform{ir::Point2D{2.0, -3.0}};
    const ir::Point2D out = transform.transformPoint(ir::Point2D{1.0, 4.0});
    REQUIRE_THAT(out.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(1.0, kTol));
}

TEST_CASE("Translation2D zero offset acts as identity on points", "[transform][Translation2D]") {
    const ir::Translation2D transform{ir::Point2D{0.0, 0.0}};
    const ir::Point2D p{7.0, -9.0};
    const ir::Point2D out = transform.transformPoint(p);
    REQUIRE_THAT(out.x(), WithinAbs(7.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(-9.0, kTol));
}

TEST_CASE("Translation2D is always invertible", "[transform][Translation2D]") {
    const ir::Translation2D a;
    const ir::Translation2D b{ir::Point2D{10.0, 20.0}};
    REQUIRE(a.isInvertible());
    REQUIRE(b.isInvertible());
}

TEST_CASE("Translation2D is a final concrete Transform2D", "[transform][Translation2D]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Transform2D, ir::Translation2D>);
    STATIC_REQUIRE(std::is_final_v<ir::Translation2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::Translation2D>);
}

TEST_CASE("Translation2D works through Transform2D base reference", "[transform][Translation2D]") {
    const ir::Translation2D concrete{ir::Point2D{1.0, 2.0}};
    const ir::Transform2D& base = concrete;
    const ir::Point2D out = base.transformPoint(ir::Point2D{3.0, 4.0});
    REQUIRE_THAT(out.x(), WithinAbs(4.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(6.0, kTol));
    REQUIRE(base.isInvertible());
}

TEST_CASE("Translation2D unique_ptr via Transform2D base", "[transform][Translation2D]") {
    std::unique_ptr<ir::Transform2D> xfm =
        std::make_unique<ir::Translation2D>(ir::Point2D{-1.0, 0.5});
    const ir::Point2D out = xfm->transformPoint(ir::Point2D{2.0, 2.0});
    REQUIRE_THAT(out.x(), WithinAbs(1.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(2.5, kTol));
}

TEST_CASE("Translation2D is copy constructible and independent", "[transform][Translation2D]") {
    const ir::Translation2D original{ir::Point2D{1.0, 1.0}};
    ir::Translation2D copy{original};
    REQUIRE_THAT(copy.offset().x(), WithinAbs(1.0, kTol));
    REQUIRE_THAT(copy.transformPoint(ir::Point2D{0.0, 0.0}).x(), WithinAbs(1.0, kTol));
}
