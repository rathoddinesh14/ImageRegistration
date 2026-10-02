#include <cmath>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kTol = 1e-12;

[[nodiscard]] ir::ImageGeometry2D makeGeometry(int width, int height) {
    return ir::ImageGeometry2D{width, height, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
}

} // namespace

TEST_CASE("Image2D construction from geometry allocates zero-filled buffer", "[core][Image2D]") {
    const ir::ImageGeometry2D geom = makeGeometry(3, 2);
    const ir::Image2D image{geom};

    REQUIRE(image.width() == 3);
    REQUIRE(image.height() == 2);
    REQUIRE(image.pixelCount() == 6);
    REQUIRE(image.geometry().width() == 3);
    REQUIRE(image.geometry().height() == 2);

    for (int j = 0; j < image.height(); ++j) {
        for (int i = 0; i < image.width(); ++i) {
            REQUIRE_THAT(image.at(i, j), WithinAbs(0.0, kTol));
        }
    }
}

TEST_CASE("Image2D construction from geometry and pixel buffer uses provided values",
          "[core][Image2D]") {
    const ir::ImageGeometry2D geom = makeGeometry(2, 2);
    // Row-major: (0,0), (1,0), (0,1), (1,1)
    const std::vector<double> pixels{1.0, 2.0, 3.0, 4.0};
    const ir::Image2D image{geom, pixels};

    REQUIRE_THAT(image.at(0, 0), WithinAbs(1.0, kTol));
    REQUIRE_THAT(image.at(1, 0), WithinAbs(2.0, kTol));
    REQUIRE_THAT(image.at(0, 1), WithinAbs(3.0, kTol));
    REQUIRE_THAT(image.at(1, 1), WithinAbs(4.0, kTol));
}

TEST_CASE("Image2D operator() matches at for read access", "[core][Image2D]") {
    const ir::ImageGeometry2D geom = makeGeometry(2, 1);
    ir::Image2D image{geom, std::vector<double>{5.5, 6.5}};

    REQUIRE_THAT(image(0, 0), WithinAbs(image.at(0, 0), kTol));
    REQUIRE_THAT(image(1, 0), WithinAbs(image.at(1, 0), kTol));
}

TEST_CASE("Image2D non-const at allows in-place pixel writes", "[core][Image2D]") {
    ir::Image2D image{makeGeometry(2, 2)};

    image.at(0, 0) = 10.0;
    image.at(1, 1) = 20.0;

    REQUIRE_THAT(image.at(0, 0), WithinAbs(10.0, kTol));
    REQUIRE_THAT(image.at(1, 0), WithinAbs(0.0, kTol));
    REQUIRE_THAT(image.at(0, 1), WithinAbs(0.0, kTol));
    REQUIRE_THAT(image.at(1, 1), WithinAbs(20.0, kTol));
}

TEST_CASE("Image2D non-const operator() allows in-place pixel writes", "[core][Image2D]") {
    ir::Image2D image{makeGeometry(1, 2)};

    image(0, 0) = -1.5;
    image(0, 1) = 3.25;

    REQUIRE_THAT(image.at(0, 0), WithinAbs(-1.5, kTol));
    REQUIRE_THAT(image.at(0, 1), WithinAbs(3.25, kTol));
}

TEST_CASE("Image2D fill sets every pixel to the given value", "[core][Image2D]") {
    ir::Image2D image{makeGeometry(4, 3)};
    image.fill(7.0);

    for (int j = 0; j < image.height(); ++j) {
        for (int i = 0; i < image.width(); ++i) {
            REQUIRE_THAT(image.at(i, j), WithinAbs(7.0, kTol));
        }
    }
}

TEST_CASE("Image2D containsIndex delegates to geometry bounds", "[core][Image2D]") {
    const ir::Image2D image{makeGeometry(3, 2)};

    REQUIRE(image.containsIndex(0, 0));
    REQUIRE(image.containsIndex(2, 1));
    REQUIRE_FALSE(image.containsIndex(-1, 0));
    REQUIRE_FALSE(image.containsIndex(0, -1));
    REQUIRE_FALSE(image.containsIndex(3, 0));
    REQUIRE_FALSE(image.containsIndex(0, 2));
}

TEST_CASE("Image2D single-pixel image supports read and write", "[core][Image2D]") {
    ir::Image2D image{makeGeometry(1, 1)};

    REQUIRE(image.pixelCount() == 1);
    REQUIRE_THAT(image.at(0, 0), WithinAbs(0.0, kTol));

    image.at(0, 0) = 42.0;
    REQUIRE_THAT(image(0, 0), WithinAbs(42.0, kTol));
}

TEST_CASE("Image2D wide and tall shapes store independent pixels", "[core][Image2D]") {
    ir::Image2D wide{makeGeometry(16, 1)};
    ir::Image2D tall{makeGeometry(1, 16)};

    wide.at(15, 0) = 1.0;
    tall.at(0, 15) = 2.0;

    REQUIRE_THAT(wide.at(0, 0), WithinAbs(0.0, kTol));
    REQUIRE_THAT(wide.at(15, 0), WithinAbs(1.0, kTol));
    REQUIRE_THAT(tall.at(0, 0), WithinAbs(0.0, kTol));
    REQUIRE_THAT(tall.at(0, 15), WithinAbs(2.0, kTol));
}

TEST_CASE("Image2D row-major layout maps linear index j*width+i", "[core][Image2D]") {
    const ir::ImageGeometry2D geom = makeGeometry(3, 2);
    // values chosen so linear position is obvious: 100*linear + 1
    std::vector<double> pixels(6);
    for (int j = 0; j < 2; ++j) {
        for (int i = 0; i < 3; ++i) {
            pixels[static_cast<std::size_t>(j * 3 + i)] = static_cast<double>(j * 3 + i);
        }
    }
    const ir::Image2D image{geom, pixels};

    REQUIRE_THAT(image.at(0, 0), WithinAbs(0.0, kTol));
    REQUIRE_THAT(image.at(1, 0), WithinAbs(1.0, kTol));
    REQUIRE_THAT(image.at(2, 0), WithinAbs(2.0, kTol));
    REQUIRE_THAT(image.at(0, 1), WithinAbs(3.0, kTol));
    REQUIRE_THAT(image.at(1, 1), WithinAbs(4.0, kTol));
    REQUIRE_THAT(image.at(2, 1), WithinAbs(5.0, kTol));
}

TEST_CASE("Image2D data view exposes contiguous buffer of pixelCount length", "[core][Image2D]") {
    ir::Image2D image{makeGeometry(2, 2)};
    image.at(0, 0) = 1.0;
    image.at(1, 0) = 2.0;
    image.at(0, 1) = 3.0;
    image.at(1, 1) = 4.0;

    REQUIRE(image.dataSize() == static_cast<std::size_t>(image.pixelCount()));
    REQUIRE(image.data() != nullptr);
    REQUIRE_THAT(image.data()[0], WithinAbs(1.0, kTol));
    REQUIRE_THAT(image.data()[1], WithinAbs(2.0, kTol));
    REQUIRE_THAT(image.data()[2], WithinAbs(3.0, kTol));
    REQUIRE_THAT(image.data()[3], WithinAbs(4.0, kTol));
}

TEST_CASE("Image2D const data view is read-only accessible", "[core][Image2D]") {
    const ir::Image2D image{makeGeometry(2, 1), std::vector<double>{9.0, 8.0}};

    const double* data = image.data();
    REQUIRE(data != nullptr);
    REQUIRE_THAT(data[0], WithinAbs(9.0, kTol));
    REQUIRE_THAT(data[1], WithinAbs(8.0, kTol));
}

TEST_CASE("Image2D copy constructs independent pixel storage", "[core][Image2D]") {
    ir::Image2D original{makeGeometry(2, 1)};
    original.at(0, 0) = 1.0;
    original.at(1, 0) = 2.0;

    ir::Image2D copy{original};
    copy.at(0, 0) = 99.0;

    REQUIRE_THAT(original.at(0, 0), WithinAbs(1.0, kTol));
    REQUIRE_THAT(copy.at(0, 0), WithinAbs(99.0, kTol));
    REQUIRE_THAT(copy.at(1, 0), WithinAbs(2.0, kTol));
}

TEST_CASE("Image2D move construction transfers pixels and leaves source empty buffer",
          "[core][Image2D]") {
    ir::Image2D original{makeGeometry(2, 1), std::vector<double>{3.0, 4.0}};
    ir::Image2D moved{std::move(original)};

    REQUIRE(moved.width() == 2);
    REQUIRE_THAT(moved.at(0, 0), WithinAbs(3.0, kTol));
    REQUIRE_THAT(moved.at(1, 0), WithinAbs(4.0, kTol));
}

TEST_CASE("Image2D geometry matches constructor geometry including spacing and origin",
          "[core][Image2D]") {
    const ir::ImageGeometry2D geom{4, 5, ir::Point2D{0.5, 1.5}, ir::Point2D{10.0, -2.0}};
    const ir::Image2D image{geom};

    REQUIRE(image.geometry().width() == 4);
    REQUIRE(image.geometry().height() == 5);
    REQUIRE_THAT(image.geometry().spacing().x(), WithinAbs(0.5, kTol));
    REQUIRE_THAT(image.geometry().spacing().y(), WithinAbs(1.5, kTol));
    REQUIRE_THAT(image.geometry().origin().x(), WithinAbs(10.0, kTol));
    REQUIRE_THAT(image.geometry().origin().y(), WithinAbs(-2.0, kTol));
}

TEST_CASE("Image2D supports negative and non-finite pixel values as plain data",
          "[core][Image2D]") {
    ir::Image2D image{makeGeometry(2, 1)};
    image.at(0, 0) = -0.0;
    image.at(1, 0) = 1e300;

    REQUIRE_THAT(image.at(0, 0), WithinAbs(0.0, kTol));
    REQUIRE(image.at(1, 0) > 1e299);
}

TEST_CASE("Image2D is copy and move constructible", "[core][Image2D]") {
    STATIC_REQUIRE(std::is_copy_constructible_v<ir::Image2D>);
    STATIC_REQUIRE(std::is_move_constructible_v<ir::Image2D>);
    STATIC_REQUIRE(std::is_copy_assignable_v<ir::Image2D>);
    STATIC_REQUIRE(std::is_move_assignable_v<ir::Image2D>);
}
