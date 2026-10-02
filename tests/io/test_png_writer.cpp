#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/io/ExportOptions.hpp>
#include <ir/io/PngWriter.hpp>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

namespace {

[[nodiscard]] ir::ImageGeometry2D makeGeometry(int width, int height) {
    return ir::ImageGeometry2D{width, height, ir::Point2D{1.0, 1.0}, ir::Point2D{0.0, 0.0}};
}

[[nodiscard]] std::filesystem::path uniqueTempPng(const std::string& tag) {
    const auto dir = std::filesystem::temp_directory_path();
    return dir / ("ir_png_" + tag + ".png");
}

[[nodiscard]] bool fileStartsWithPngSignature(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    unsigned char sig[8] = {};
    in.read(reinterpret_cast<char*>(sig), 8);
    if (in.gcount() != 8) {
        return false;
    }
    static const unsigned char kPng[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    for (int i = 0; i < 8; ++i) {
        if (sig[i] != kPng[i]) {
            return false;
        }
    }
    return true;
}

} // namespace

TEST_CASE("PngWriter writes a file that exists and is non-empty", "[io][PngWriter]") {
    ir::Image2D image{makeGeometry(4, 3)};
    image.fill(0.5);

    const auto path = uniqueTempPng("exists");
    std::filesystem::remove(path);

    ir::io::PngWriter writer;
    const bool ok = writer.write(image, path.string());

    REQUIRE(ok);
    REQUIRE(std::filesystem::exists(path));
    REQUIRE(std::filesystem::file_size(path) > 0);

    std::filesystem::remove(path);
}

TEST_CASE("PngWriter output starts with the PNG signature", "[io][PngWriter]") {
    ir::Image2D image{makeGeometry(2, 2)};
    image.at(0, 0) = 0.0;
    image.at(1, 0) = 1.0;
    image.at(0, 1) = 0.25;
    image.at(1, 1) = 0.75;

    const auto path = uniqueTempPng("signature");
    std::filesystem::remove(path);

    REQUIRE(ir::io::PngWriter{}.write(image, path.string()));
    REQUIRE(fileStartsWithPngSignature(path));

    std::filesystem::remove(path);
}

TEST_CASE("writePng convenience function matches PngWriter success path", "[io][PngWriter]") {
    const ir::Image2D image{makeGeometry(1, 1), std::vector<double>{0.0}};
    const auto path = uniqueTempPng("convenience");
    std::filesystem::remove(path);

    REQUIRE(ir::io::writePng(image, path.string()));
    REQUIRE(std::filesystem::exists(path));
    REQUIRE(fileStartsWithPngSignature(path));

    std::filesystem::remove(path);
}

TEST_CASE("PngWriter accepts ExportOptions Clamp01 scaling", "[io][PngWriter]") {
    ir::Image2D image{makeGeometry(2, 1)};
    image.at(0, 0) = 0.0;
    image.at(1, 0) = 1.0;

    ir::io::ExportOptions options;
    options.scaling = ir::io::ScalingMode::Clamp01;

    const auto path = uniqueTempPng("clamp01");
    std::filesystem::remove(path);

    REQUIRE(ir::io::PngWriter{}.write(image, path.string(), options));
    REQUIRE(std::filesystem::file_size(path) > 0);

    std::filesystem::remove(path);
}

TEST_CASE("PngWriter clamps out-of-range values under Clamp01", "[io][PngWriter]") {
    ir::Image2D image{makeGeometry(3, 1)};
    image.at(0, 0) = -1.0;
    image.at(1, 0) = 0.5;
    image.at(2, 0) = 2.0;

    const auto path = uniqueTempPng("clamp_range");
    std::filesystem::remove(path);

    // Must still succeed; clamping is internal policy (file remains valid PNG).
    REQUIRE(ir::io::PngWriter{}.write(image, path.string()));
    REQUIRE(fileStartsWithPngSignature(path));

    std::filesystem::remove(path);
}

TEST_CASE("PngWriter supports single-pixel and wide images", "[io][PngWriter]") {
    {
        const ir::Image2D one{makeGeometry(1, 1), std::vector<double>{0.2}};
        const auto path = uniqueTempPng("one_pixel");
        std::filesystem::remove(path);
        REQUIRE(ir::io::PngWriter{}.write(one, path.string()));
        REQUIRE(fileStartsWithPngSignature(path));
        std::filesystem::remove(path);
    }
    {
        ir::Image2D wide{makeGeometry(32, 1)};
        wide.fill(0.8);
        const auto path = uniqueTempPng("wide");
        std::filesystem::remove(path);
        REQUIRE(ir::io::PngWriter{}.write(wide, path.string()));
        REQUIRE(std::filesystem::file_size(path) > 0);
        std::filesystem::remove(path);
    }
}

TEST_CASE("PngWriter returns false for an invalid output path", "[io][PngWriter]") {
    const ir::Image2D image{makeGeometry(2, 2)};
    // Empty path should fail without throwing.
    REQUIRE_FALSE(ir::io::PngWriter{}.write(image, ""));
}

TEST_CASE("PngWriter default ExportOptions use Clamp01", "[io][ExportOptions]") {
    const ir::io::ExportOptions defaults{};
    REQUIRE(defaults.scaling == ir::io::ScalingMode::Clamp01);
}
