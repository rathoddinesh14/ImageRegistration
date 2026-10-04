#include <cmath>
#include <ir/core/Point2D.hpp>
#include <ir/transform/Rigid2D.hpp>
#include <ir/transform/Transform2D.hpp>
#include <memory>
#include <numbers>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

} // namespace

TEST_CASE("Rigid2D default is identity", "[transform][Rigid2D]") {
    const ir::Rigid2D rigid;
    REQUIRE_THAT(rigid.angle(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(rigid.translation().x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(rigid.translation().y(), WithinAbs(0.0, kTol));

    const ir::Point2D p{3.0, -4.0};
    const ir::Point2D out = rigid.transformPoint(p);
    REQUIRE_THAT(out.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(-4.0, kTol));
}

TEST_CASE("Rigid2D pure translation matches offset", "[transform][Rigid2D]") {
    const ir::Rigid2D rigid{0.0, ir::Point2D{2.0, -1.0}};
    const ir::Point2D out = rigid.transformPoint(ir::Point2D{1.0, 1.0});
    REQUIRE_THAT(out.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(0.0, kTol));
}

TEST_CASE("Rigid2D pure rotation by pi/2 about origin", "[transform][Rigid2D]") {
    const double halfPi = std::numbers::pi_v<double> / 2.0;
    const ir::Rigid2D rigid{halfPi, ir::Point2D{0.0, 0.0}};

    // (1, 0) -> (0, 1)
    const ir::Point2D a = rigid.transformPoint(ir::Point2D{1.0, 0.0});
    REQUIRE_THAT(a.x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(a.y(), WithinAbs(1.0, kTol));

    // (0, 1) -> (-1, 0)
    const ir::Point2D b = rigid.transformPoint(ir::Point2D{0.0, 1.0});
    REQUIRE_THAT(b.x(), WithinAbs(-1.0, kTol));
    REQUIRE_THAT(b.y(), WithinAbs(0.0, kTol));
}

TEST_CASE("Rigid2D rotate then translate", "[transform][Rigid2D]") {
    const double halfPi = std::numbers::pi_v<double> / 2.0;
    const ir::Rigid2D rigid{halfPi, ir::Point2D{10.0, 20.0}};

    // (1, 0) -> (0, 1) + (10, 20) = (10, 21)
    const ir::Point2D out = rigid.transformPoint(ir::Point2D{1.0, 0.0});
    REQUIRE_THAT(out.x(), WithinAbs(10.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(21.0, kTol));
}

TEST_CASE("Rigid2D is always invertible", "[transform][Rigid2D]") {
    const ir::Rigid2D a;
    const ir::Rigid2D b{0.3, ir::Point2D{1.0, 2.0}};
    REQUIRE(a.isInvertible());
    REQUIRE(b.isInvertible());
}

TEST_CASE("Rigid2D is final concrete Transform2D", "[transform][Rigid2D]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Transform2D, ir::Rigid2D>);
    STATIC_REQUIRE(std::is_final_v<ir::Rigid2D>);
    STATIC_REQUIRE(!std::is_abstract_v<ir::Rigid2D>);
}

TEST_CASE("Rigid2D works through Transform2D base", "[transform][Rigid2D]") {
    const ir::Rigid2D concrete{0.0, ir::Point2D{1.0, 2.0}};
    const ir::Transform2D& base = concrete;
    const ir::Point2D out = base.transformPoint(ir::Point2D{0.0, 0.0});
    REQUIRE_THAT(out.x(), WithinAbs(1.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(2.0, kTol));
    REQUIRE(base.isInvertible());
}

TEST_CASE("Rigid2D unique_ptr via Transform2D base", "[transform][Rigid2D]") {
    std::unique_ptr<ir::Transform2D> xfm =
        std::make_unique<ir::Rigid2D>(0.0, ir::Point2D{-1.0, 0.5});
    const ir::Point2D out = xfm->transformPoint(ir::Point2D{2.0, 2.0});
    REQUIRE_THAT(out.x(), WithinAbs(1.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(2.5, kTol));
}

TEST_CASE("Rigid2D stores angle and translation accessors", "[transform][Rigid2D]") {
    const ir::Rigid2D rigid{1.25, ir::Point2D{3.0, 4.0}};
    REQUIRE_THAT(rigid.angle(), WithinAbs(1.25, kTol));
    REQUIRE_THAT(rigid.translation().x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(rigid.translation().y(), WithinAbs(4.0, kTol));
}
