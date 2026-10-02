#pragma once

namespace ir::io {

/**
 * How double pixel values are converted to 8-bit grayscale samples.
 */
enum class ScalingMode {
    /**
     * Clamp each value to [0, 1], then scale to 0..255.
     * Suitable for synthetic images stored as normalized intensities.
     */
    Clamp01
};

/**
 * Shared options for image export writers.
 */
struct ExportOptions {
    /** Pixel value scaling policy (default: Clamp01). */
    ScalingMode scaling = ScalingMode::Clamp01;
};

} // namespace ir::io
