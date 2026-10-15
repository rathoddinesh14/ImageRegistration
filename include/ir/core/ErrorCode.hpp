#pragma once

namespace ir {

/**
 * Stable, small set of recoverable failure codes for library APIs.
 *
 * Used by RegistrationResult and (later) ir::Expected / Error. Prefer adding
 * codes only when a real API needs them.
 */
enum class ErrorCode {
    /** No error / success. */
    Ok = 0,
    /** Transform linear part is singular or not invertible. */
    SingularTransform,
    /** Fixed and moving have no usable overlapping samples. */
    EmptyOverlap,
    /** Metric evaluation produced NaN or Inf. */
    NonFiniteMetric,
    /** Geometry violates library invariants (e.g. empty size). */
    InvalidGeometry,
    /** Image sizes or lattices are incompatible for the operation. */
    SizeMismatch,
    /** Requested feature is not implemented yet. */
    NotImplemented,
    /** Unclassified failure. */
    Unknown
};

} // namespace ir
