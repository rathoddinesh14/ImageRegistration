#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>

#include <cmath>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-9;

[[nodiscard]] bool pointsApproxEqual(const ir::Point2D& a, const ir::Point2D& b,
                                     double tol = kTol) {
    return std::abs(a.x() - b.x()) <= tol && std::abs(a.y() - b.y()) <= tol;
}

[[nodiscard]] Eigen::Matrix2d identityDirection() {
    return Eigen::Matrix2d::Identity();
}

/// 90 degree counter-clockwise rotation in the plane.
[[nodiscard]] Eigen::Matrix2d rotation90CcW() {
    Eigen::Matrix2d d;
    d << 0.0, -1.0, 1.0, 0.0;
    return d;
}

} // namespace

TEST_CASE("ImageGeometry2D stores size spacing origin and identity direction by default",
          "[core][ImageGeometry2D]") {
    const ir::Point2D spacing{0.5, 1.5};
    const ir::Point2D origin{10.0, 20.0};
    const ir::ImageGeometry2D geom{4, 6, spacing, origin};

    REQUIRE(geom.width() == 4);
    REQUIRE(geom.height() == 6);
    REQUIRE_THAT(geom.spacing().x(), WithinAbs(0.5, kTol));
    REQUIRE_THAT(geom.spacing().y(), WithinAbs(1.5, kTol));
    REQUIRE_THAT(geom.origin().x(), WithinAbs(10.0, kTol));
    REQUIRE_THAT(geom.origin().y(), WithinAbs(20.0, kTol));
    REQUIRE(geom.direction().isApprox(identityDirection(), kTol));
}

TEST_CASE("ImageGeometry2D accepts an explicit direction matrix", "[core][ImageGeometry2D]") {
    const Eigen::Matrix2d dir = rotation90CcW();
    const ir::ImageGeometry2D geom{2, 3, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}, dir};

    REQUIRE(geom.direction().isApprox(dir, kTol));
}

TEST_CASE("ImageGeometry2D index (0,0) maps to origin as pixel center",
          "[core][ImageGeometry2D]") {
    const ir::Point2D origin{3.0, -4.0};
    const ir::ImageGeometry2D geom{5, 7, ir::Point2D{1.0, 1.0}, origin};

    const ir::Point2D physical = geom.indexToPhysical(0, 0);

    REQUIRE_THAT(physical.x(), WithinAbs(origin.x(), kTol));
    REQUIRE_THAT(physical.y(), WithinAbs(origin.y(), kTol));
}

TEST_CASE("ImageGeometry2D indexToPhysical uses spacing along identity axes",
          "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D geom{10, 10, ir::Point2D{2.0, 3.0}, ir::Point2D{1.0, 5.0}};

    // Center of pixel (2, 1): origin + (2*sx, 1*sy)
    const ir::Point2D physical = geom.indexToPhysical(2, 1);

    REQUIRE_THAT(physical.x(), WithinAbs(1.0 + 2.0 * 2.0, kTol));
    REQUIRE_THAT(physical.y(), WithinAbs(5.0 + 1.0 * 3.0, kTol));
}

TEST_CASE("ImageGeometry2D indexToPhysical covers first and last indices along width and height",
          "[core][ImageGeometry2D]") {
    constexpr int width = 8;
    constexpr int height = 5;
    const ir::Point2D spacing{0.25, 0.5};
    const ir::Point2D origin{0.0, 0.0};
    const ir::ImageGeometry2D geom{width, height, spacing, origin};

    const ir::Point2D topLeft = geom.indexToPhysical(0, 0);
    const ir::Point2D topRight = geom.indexToPhysical(width - 1, 0);
    const ir::Point2D bottomLeft = geom.indexToPhysical(0, height - 1);
    const ir::Point2D bottomRight = geom.indexToPhysical(width - 1, height - 1);

    REQUIRE_THAT(topLeft.x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(topLeft.y(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(topRight.x(), WithinAbs((width - 1) * spacing.x(), kTol));
    REQUIRE_THAT(topRight.y(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(bottomLeft.x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(bottomLeft.y(), WithinAbs((height - 1) * spacing.y(), kTol));
    REQUIRE_THAT(bottomRight.x(), WithinAbs((width - 1) * spacing.x(), kTol));
    REQUIRE_THAT(bottomRight.y(), WithinAbs((height - 1) * spacing.y(), kTol));
}

TEST_CASE("ImageGeometry2D physicalToIndex is inverse of indexToPhysical for identity direction",
          "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D geom{16, 12, ir::Point2D{0.5, 1.25}, ir::Point2D{-2.0, 3.5}};

    for (int j = 0; j < geom.height(); j += 3) {
        for (int i = 0; i < geom.width(); i += 4) {
            const ir::Point2D physical = geom.indexToPhysical(i, j);
            const ir::Point2D index = geom.physicalToIndex(physical);
            REQUIRE_THAT(index.x(), WithinAbs(static_cast<double>(i), kTol));
            REQUIRE_THAT(index.y(), WithinAbs(static_cast<double>(j), kTol));
        }
    }
}

TEST_CASE("ImageGeometry2D round-trip holds for anisotropic spacing", "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D geom{3, 9, ir::Point2D{0.1, 4.0}, ir::Point2D{100.0, -50.0}};

    const ir::Point2D physical = geom.indexToPhysical(2, 8);
    const ir::Point2D index = geom.physicalToIndex(physical);

    REQUIRE_THAT(index.x(), WithinAbs(2.0, kTol));
    REQUIRE_THAT(index.y(), WithinAbs(8.0, kTol));
}

TEST_CASE("ImageGeometry2D applies full 2x2 direction when mapping index to physical",
          "[core][ImageGeometry2D]") {
    // direction columns: e0 = (0,1), e1 = (-1, 0)  => 90 deg CCW of identity basis
    const Eigen::Matrix2d dir = rotation90CcW();
    const ir::ImageGeometry2D geom{4, 4, ir::Point2D{2.0, 3.0}, ir::Point2D{1.0, 1.0}, dir};

    // physical = origin + D * (i*sx, j*sy) = (1,1) + D * (2, 0) for (i,j)=(1,0)
    // D * (2, 0) = (0, 2)
    const ir::Point2D physical = geom.indexToPhysical(1, 0);

    REQUIRE_THAT(physical.x(), WithinAbs(1.0 + 0.0, kTol));
    REQUIRE_THAT(physical.y(), WithinAbs(1.0 + 2.0, kTol));
}

TEST_CASE("ImageGeometry2D round-trip holds with non-identity direction",
          "[core][ImageGeometry2D]") {
    const Eigen::Matrix2d dir = rotation90CcW();
    const ir::ImageGeometry2D geom{5, 5, ir::Point2D{1.0, 2.0}, ir::Point2D{0.0, 0.0}, dir};

    for (int j = 0; j < 5; ++j) {
        for (int i = 0; i < 5; ++i) {
            const ir::Point2D physical = geom.indexToPhysical(i, j);
            const ir::Point2D index = geom.physicalToIndex(physical);
            REQUIRE_THAT(index.x(), WithinAbs(static_cast<double>(i), kTol));
            REQUIRE_THAT(index.y(), WithinAbs(static_cast<double>(j), kTol));
        }
    }
}

TEST_CASE("ImageGeometry2D containsIndex is true only inside the valid integer lattice",
          "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D geom{3, 2, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};

    REQUIRE(geom.containsIndex(0, 0));
    REQUIRE(geom.containsIndex(2, 1));
    REQUIRE(geom.containsIndex(1, 0));

    REQUIRE_FALSE(geom.containsIndex(-1, 0));
    REQUIRE_FALSE(geom.containsIndex(0, -1));
    REQUIRE_FALSE(geom.containsIndex(3, 0));
    REQUIRE_FALSE(geom.containsIndex(0, 2));
    REQUIRE_FALSE(geom.containsIndex(3, 2));
}

TEST_CASE("ImageGeometry2D supports single-pixel geometry", "[core][ImageGeometry2D]") {
    const ir::Point2D origin{7.0, 8.0};
    const ir::ImageGeometry2D geom{1, 1, ir::Point2D{2.0, 3.0}, origin};

    REQUIRE(geom.width() == 1);
    REQUIRE(geom.height() == 1);
    REQUIRE(geom.containsIndex(0, 0));
    REQUIRE_FALSE(geom.containsIndex(1, 0));
    REQUIRE_FALSE(geom.containsIndex(0, 1));

    const ir::Point2D physical = geom.indexToPhysical(0, 0);
    REQUIRE(pointsApproxEqual(physical, origin));

    const ir::Point2D index = geom.physicalToIndex(origin);
    REQUIRE_THAT(index.x(), WithinAbs(0.0, kTol));
    REQUIRE_THAT(index.y(), WithinAbs(0.0, kTol));
}

TEST_CASE("ImageGeometry2D supports wide and tall aspect ratios", "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D wide{64, 2, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
    const ir::ImageGeometry2D tall{2, 64, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};

    REQUIRE(wide.width() == 64);
    REQUIRE(wide.height() == 2);
    REQUIRE(tall.width() == 2);
    REQUIRE(tall.height() == 64);

    REQUIRE(pointsApproxEqual(wide.indexToPhysical(63, 1), ir::Point2D{63.0, 1.0}));
    REQUIRE(pointsApproxEqual(tall.indexToPhysical(1, 63), ir::Point2D{1.0, 63.0}));
}

TEST_CASE("ImageGeometry2D equality compares size spacing origin and direction",
          "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D a{4, 5, ir::Point2D{1.0, 2.0}, ir::Point2D{3.0, 4.0}};
    const ir::ImageGeometry2D b{4, 5, ir::Point2D{1.0, 2.0}, ir::Point2D{3.0, 4.0}};
    const ir::ImageGeometry2D differentSize{5, 5, ir::Point2D{1.0, 2.0}, ir::Point2D{3.0, 4.0}};
    const ir::ImageGeometry2D differentOrigin{4, 5, ir::Point2D{1.0, 2.0}, ir::Point2D{0.0, 4.0}};

    REQUIRE(a == b);
    REQUIRE_FALSE(a != b);
    REQUIRE_FALSE(a == differentSize);
    REQUIRE(a != differentOrigin);
}

TEST_CASE("ImageGeometry2D is a nothrow copyable value type", "[core][ImageGeometry2D]") {
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<ir::ImageGeometry2D>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<ir::ImageGeometry2D>);
    STATIC_REQUIRE(std::is_copy_assignable_v<ir::ImageGeometry2D>);
}

TEST_CASE("ImageGeometry2D physicalToIndex returns fractional indices between pixel centers",
          "[core][ImageGeometry2D]") {
    const ir::ImageGeometry2D geom{4, 4, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};

    // Midway between centers of (0,0) and (1,0)
    const ir::Point2D index = geom.physicalToIndex(ir::Point2D{0.5, 0.0});

    REQUIRE_THAT(index.x(), WithinAbs(0.5, kTol));
    REQUIRE_THAT(index.y(), WithinAbs(0.0, kTol));
}
