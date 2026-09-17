#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <ir/core/Point2D.hpp>

#include <type_traits>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

} // namespace

TEST_CASE("Point2D default construction yields origin", "[core][Point2D]") {
    const ir::Point2D p{};

    REQUIRE_THAT(p.x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(p.y(), WithinAbs(0.0, kTol));
}

TEST_CASE("Point2D can be constructed from x and y", "[core][Point2D]") {
    const ir::Point2D p{3.5, -2.0};

    REQUIRE_THAT(p.x(), WithinAbs(3.5, kTol));
    REQUIRE_THAT(p.y(), WithinAbs(-2.0, kTol));
}

TEST_CASE("Point2D accessors return independent coordinates", "[core][Point2D]") {
    const ir::Point2D p{1.25, 4.5};

    REQUIRE_THAT(p.x(), WithinAbs(1.25, kTol));
    REQUIRE_THAT(p.y(), WithinAbs(4.5, kTol));
    REQUIRE(p.x() != p.y());
}

TEST_CASE("Point2D addition adds coordinates componentwise", "[core][Point2D]") {
    const ir::Point2D a{1.0, 2.0};
    const ir::Point2D b{3.0, 4.0};

    const ir::Point2D sum = a + b;

    REQUIRE_THAT(sum.x(), WithinAbs(4.0, kTol));
    REQUIRE_THAT(sum.y(), WithinAbs(6.0, kTol));
}

TEST_CASE("Point2D subtraction subtracts coordinates componentwise", "[core][Point2D]") {
    const ir::Point2D a{5.0, 7.0};
    const ir::Point2D b{2.0, 3.0};

    const ir::Point2D diff = a - b;

    REQUIRE_THAT(diff.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(diff.y(), WithinAbs(4.0, kTol));
}

TEST_CASE("Point2D compound addition updates the left operand", "[core][Point2D]") {
    ir::Point2D p{1.0, 2.0};
    p += ir::Point2D{0.5, -1.0};

    REQUIRE_THAT(p.x(), WithinAbs(1.5, kTol));
    REQUIRE_THAT(p.y(), WithinAbs(1.0, kTol));
}

TEST_CASE("Point2D compound subtraction updates the left operand", "[core][Point2D]") {
    ir::Point2D p{4.0, 6.0};
    p -= ir::Point2D{1.0, 2.0};

    REQUIRE_THAT(p.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(p.y(), WithinAbs(4.0, kTol));
}

TEST_CASE("Point2D scalar multiplication scales both coordinates", "[core][Point2D]") {
    const ir::Point2D p{2.0, -3.0};

    const ir::Point2D scaled = p * 2.0;

    REQUIRE_THAT(scaled.x(), WithinAbs(4.0, kTol));
    REQUIRE_THAT(scaled.y(), WithinAbs(-6.0, kTol));
}

TEST_CASE("Point2D scalar multiplication is commutative", "[core][Point2D]") {
    const ir::Point2D p{2.0, -3.0};

    const ir::Point2D left = 2.0 * p;
    const ir::Point2D right = p * 2.0;

    REQUIRE_THAT(left.x(), WithinAbs(right.x(), kTol));
    REQUIRE_THAT(left.y(), WithinAbs(right.y(), kTol));
}

TEST_CASE("Point2D unary minus negates coordinates", "[core][Point2D]") {
    const ir::Point2D p{1.5, -2.5};
    const ir::Point2D n = -p;

    REQUIRE_THAT(n.x(), WithinAbs(-1.5, kTol));
    REQUIRE_THAT(n.y(), WithinAbs(2.5, kTol));
}

TEST_CASE("Point2D equality compares coordinates", "[core][Point2D]") {
    const ir::Point2D a{1.0, 2.0};
    const ir::Point2D b{1.0, 2.0};
    const ir::Point2D c{1.0, 2.5};

    REQUIRE(a == b);
    REQUIRE_FALSE(a == c);
    REQUIRE(a != c);
    REQUIRE_FALSE(a != b);
}

TEST_CASE("Point2D is a trivial value type with no dynamic allocation API", "[core][Point2D]") {
    STATIC_REQUIRE(std::is_trivially_copyable_v<ir::Point2D>);
    STATIC_REQUIRE(std::is_nothrow_default_constructible_v<ir::Point2D>);
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<ir::Point2D>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<ir::Point2D>);
}

TEST_CASE("Point2D interops with Eigen Vector2d", "[core][Point2D][eigen]") {
    const ir::Point2D p{3.0, 4.0};
    const Eigen::Vector2d v = p.toEigen();

    REQUIRE_THAT(v.x(), WithinAbs(3.0, kTol));
    REQUIRE_THAT(v.y(), WithinAbs(4.0, kTol));

    const ir::Point2D q = ir::Point2D::fromEigen(Eigen::Vector2d{5.0, 6.0});
    REQUIRE_THAT(q.x(), WithinAbs(5.0, kTol));
    REQUIRE_THAT(q.y(), WithinAbs(6.0, kTol));
}
