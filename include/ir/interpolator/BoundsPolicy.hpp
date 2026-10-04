#pragma once

namespace ir {

/**
 * How interpolators treat sample locations outside the image lattice.
 *
 * Note: Constant(0) is convenient for demos but confuses "missing" with a real
 * intensity of zero. Prefer Clamp or a dedicated mask/Expected path for metrics
 * when those land. Constant uses a caller-chosen fill value (default 0).
 */
enum class BoundsPolicy {
    /** Return a constant fill value when out of bounds. */
    Constant,
    /** Use the nearest in-bounds integer index (edge clamp). */
    Clamp
};

} // namespace ir
