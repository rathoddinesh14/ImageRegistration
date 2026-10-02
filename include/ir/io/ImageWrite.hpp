#pragma once

#include <cctype>
#include <ir/core/Image2D.hpp>
#include <ir/io/ExportOptions.hpp>
#include <ir/io/PngWriter.hpp>
#include <string>

namespace ir::io {
namespace detail {

/**
 * @param path Filesystem path.
 * @return Lowercase extension including the dot (e.g. ".png"), or empty if none.
 */
[[nodiscard]] inline std::string fileExtensionLower(const std::string& path) {
    const auto slash = path.find_last_of("/\\");
    const auto nameBegin = (slash == std::string::npos) ? 0 : slash + 1;
    const auto dot = path.find_last_of('.');
    if (dot == std::string::npos || dot < nameBegin) {
        return {};
    }
    std::string ext = path.substr(dot);
    for (char& c : ext) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return ext;
}

} // namespace detail

/**
 * Facade: write an image by choosing a format writer from the path extension.
 *
 * v0.1 supports ".png" via PngWriter. Additional formats register here later
 * without changing Image2D or call sites that already use write().
 *
 * @param image Source image.
 * @param path Output path; extension selects the writer (e.g. out.png).
 * @param options Shared export options (scaling, ...).
 * @return True on success; false if the extension is unsupported or write fails.
 */
[[nodiscard]] inline bool write(const Image2D& image, const std::string& path,
                                const ExportOptions& options = ExportOptions{}) {
    const std::string ext = detail::fileExtensionLower(path);
    if (ext == ".png") {
        return PngWriter{}.write(image, path, options);
    }
    // Future: ".pgm" -> PgmWriter, etc.
    return false;
}

} // namespace ir::io
