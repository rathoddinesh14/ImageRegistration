#include <ir/core/Point2D.hpp>
#include <ir/transform/ParametricTransform2D.hpp>
#include <ir/transform/Rigid2D.hpp>
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

TEST_CASE("ParametricTransform2D is abstract and extends Transform2D",
          "[transform][ParametricTransform2D]") {
    STATIC_REQUIRE(std::is_abstract_v<ir::ParametricTransform2D>);
    STATIC_REQUIRE(std::is_base_of_v<ir::Transform2D, ir::ParametricTransform2D>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<ir::ParametricTransform2D>);
}

TEST_CASE("Translation2D parameter get/set round-trip", "[transform][ParametricTransform2D]") {
    ir::Translation2D t{ir::Point2D{1.5, -2.25}};
    REQUIRE(t.numberOfParameters() == 2);

    double buf[2] = {};
    t.getParameters(buf);
    REQUIRE_THAT(buf[0], WithinAbs(1.5, kTol));
    REQUIRE_THAT(buf[1], WithinAbs(-2.25, kTol));

    const double next[2] = {3.0, 4.0};
    t.setParameters(next);
    REQUIRE_THAT(t.offset().x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(t.offset().y(), WithinAbs(4.0, kTol));

    double again[2] = {};
    t.getParameters(again);
    REQUIRE_THAT(again[0], WithinAbs(3.0, kTol));
    REQUIRE_THAT(again[1], WithinAbs(4.0, kTol));
}

TEST_CASE("Translation2D parameters change transformPoint", "[transform][ParametricTransform2D]") {
    ir::Translation2D t;
    const double p[2] = {2.0, 3.0};
    t.setParameters(p);
    const ir::Point2D out = t.transformPoint(ir::Point2D{1.0, 1.0});
    REQUIRE_THAT(out.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(out.y(), WithinAbs(4.0, kTol));
}

TEST_CASE("Rigid2D parameter get/set round-trip", "[transform][ParametricTransform2D]") {
    ir::Rigid2D r{0.25, ir::Point2D{1.0, 2.0}};
    REQUIRE(r.numberOfParameters() == 3);

    double buf[3] = {};
    r.getParameters(buf);
    REQUIRE_THAT(buf[0], WithinAbs(0.25, kTol));
    REQUIRE_THAT(buf[1], WithinAbs(1.0, kTol));
    REQUIRE_THAT(buf[2], WithinAbs(2.0, kTol));

    const double next[3] = {-0.5, 7.0, 8.0};
    r.setParameters(next);
    REQUIRE_THAT(r.angle(), WithinAbs(-0.5, kTol));
    REQUIRE_THAT(r.translation().x(), WithinAbs(7.0, kTol));
    REQUIRE_THAT(r.translation().y(), WithinAbs(8.0, kTol));
}

TEST_CASE("ParametricTransform2D works through base pointer",
          "[transform][ParametricTransform2D]") {
    auto t = std::make_unique<ir::Translation2D>(ir::Point2D{9.0, 1.0});
    ir::ParametricTransform2D* p = t.get();
    REQUIRE(p->numberOfParameters() == 2);

    double buf[2] = {};
    p->getParameters(buf);
    REQUIRE_THAT(buf[0], WithinAbs(9.0, kTol));
    REQUIRE_THAT(buf[1], WithinAbs(1.0, kTol));

    const double next[2] = {0.0, 0.0};
    p->setParameters(next);
    REQUIRE_THAT(t->offset().x(), WithinAbs(0.0, kTol));
}

TEST_CASE("Translation2D and Rigid2D remain Transform2D", "[transform][ParametricTransform2D]") {
    STATIC_REQUIRE(std::is_base_of_v<ir::Transform2D, ir::Translation2D>);
    STATIC_REQUIRE(std::is_base_of_v<ir::ParametricTransform2D, ir::Translation2D>);
    STATIC_REQUIRE(std::is_base_of_v<ir::ParametricTransform2D, ir::Rigid2D>);
    STATIC_REQUIRE(std::is_final_v<ir::Translation2D>);
    STATIC_REQUIRE(std::is_final_v<ir::Rigid2D>);
}
