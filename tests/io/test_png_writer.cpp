#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ir/core/Image2D.hpp>
#include <ir/core/ImageGeometry2D.hpp>
#include <ir/core/Point2D.hpp>
#include <ir/io/ExportOptions.hpp>
#include <ir/io/ImageWrite.hpp>
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

/** Directory for committed/reviewable sample PNGs (override with IR_PNG_SAMPLE_DIR). */
[[nodiscard]] std::filesystem::path sampleOutputDir() {
#if defined(_MSC_VER)
    char* buffer = nullptr;
    size_t length = 0;
    if (_dupenv_s(&buffer, &length, "IR_PNG_SAMPLE_DIR") == 0 && buffer != nullptr) {
        std::filesystem::path path{buffer};
        free(buffer);
        return path;
    }
#else
    if (const char* env = std::getenv("IR_PNG_SAMPLE_DIR")) {
        return std::filesystem::path{env};
    }
#endif
    return std::filesystem::current_path() / "io_png_samples";
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
    REQUIRE_FALSE(ir::io::PngWriter{}.write(image, ""));
}

TEST_CASE("PngWriter default ExportOptions use Clamp01", "[io][ExportOptions]") {
    const ir::io::ExportOptions defaults{};
    REQUIRE(defaults.scaling == ir::io::ScalingMode::Clamp01);
}

TEST_CASE("PngWriter respects Image2D geometry size (width height pixelCount)",
          "[io][PngWriter][geometry]") {
    const ir::ImageGeometry2D geom{16, 10, ir::Point2D{0.5, 1.25}, ir::Point2D{-2.0, 3.0}};
    ir::Image2D image{geom};
    REQUIRE(image.width() == 16);
    REQUIRE(image.height() == 10);
    REQUIRE(image.pixelCount() == 160);
    REQUIRE(image.geometry().spacing().x() == 0.5);
    REQUIRE(image.geometry().origin().y() == 3.0);

    // Non-uniform pattern so the PNG is not a flat field
    for (int j = 0; j < image.height(); ++j) {
        for (int i = 0; i < image.width(); ++i) {
            image.at(i, j) = static_cast<double>(i) / static_cast<double>(image.width() - 1);
        }
    }

    const auto path = uniqueTempPng("geometry_size");
    std::filesystem::remove(path);
    REQUIRE(ir::io::PngWriter{}.write(image, path.string()));
    REQUIRE(fileStartsWithPngSignature(path));
    // Geometry is unchanged by export (IO must not mutate core data)
    REQUIRE(image.geometry().width() == 16);
    REQUIRE(image.geometry().height() == 10);
    REQUIRE(image.pixelCount() == 160);
    std::filesystem::remove(path);
}

TEST_CASE("PngWriter sample artifacts: gradient disk checkerboard", "[io][PngWriter][samples]") {
    const auto outDir = sampleOutputDir();
    std::filesystem::create_directories(outDir);

    // Horizontal gradient 64x32
    {
        ir::Image2D gradient{makeGeometry(64, 32)};
        for (int j = 0; j < gradient.height(); ++j) {
            for (int i = 0; i < gradient.width(); ++i) {
                gradient.at(i, j) =
                    static_cast<double>(i) / static_cast<double>(gradient.width() - 1);
            }
        }
        const auto path = outDir / "sample_gradient.png";
        REQUIRE(ir::io::writePng(gradient, path.string()));
        REQUIRE(fileStartsWithPngSignature(path));
    }

    // Bright disk on dark background 64x64
    {
        ir::Image2D disk{makeGeometry(64, 64)};
        disk.fill(0.05);
        const double cx = 32.0;
        const double cy = 32.0;
        const double radius = 18.0;
        for (int j = 0; j < disk.height(); ++j) {
            for (int i = 0; i < disk.width(); ++i) {
                const double dx = static_cast<double>(i) + 0.5 - cx;
                const double dy = static_cast<double>(j) + 0.5 - cy;
                if (dx * dx + dy * dy <= radius * radius) {
                    disk.at(i, j) = 0.95;
                }
            }
        }
        const auto path = outDir / "sample_disk.png";
        REQUIRE(ir::io::writePng(disk, path.string()));
        REQUIRE(fileStartsWithPngSignature(path));
    }

    // Checkerboard 48x48
    {
        ir::Image2D board{makeGeometry(48, 48)};
        for (int j = 0; j < board.height(); ++j) {
            for (int i = 0; i < board.width(); ++i) {
                const bool light = ((i / 8) + (j / 8)) % 2 == 0;
                board.at(i, j) = light ? 0.9 : 0.1;
            }
        }
        const auto path = outDir / "sample_checkerboard.png";
        REQUIRE(ir::io::writePng(board, path.string()));
        REQUIRE(fileStartsWithPngSignature(path));
    }
}

TEST_CASE("write facade dispatches .png to PngWriter", "[io][ImageWrite]") {
    const ir::Image2D image{makeGeometry(2, 2)};
    const auto path = uniqueTempPng("facade_png");
    std::filesystem::remove(path);

    REQUIRE(ir::io::write(image, path.string()));
    REQUIRE(fileStartsWithPngSignature(path));

    std::filesystem::remove(path);
}

TEST_CASE("write facade rejects unsupported extensions", "[io][ImageWrite]") {
    const ir::Image2D image{makeGeometry(2, 2)};
    REQUIRE_FALSE(ir::io::write(image, "out.bmp"));
    REQUIRE_FALSE(ir::io::write(image, "out"));
    REQUIRE_FALSE(ir::io::write(image, ""));
}
