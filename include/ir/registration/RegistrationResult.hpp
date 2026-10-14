#pragma once

#include <ir/core/ErrorCode.hpp>
#include <string>

namespace ir {

/**
 * Summary of a registration run (Ceres-style solve report, simplified).
 *
 * Validity:
 * - On success: succeeded == true, code == ErrorCode::Ok, finalMetric and
 *   iterations are meaningful for the completed run.
 * - On failure: succeeded == false, code != ErrorCode::Ok when a specific
 *   reason is known; message may describe the failure.
 * - Transform parameters are not stored here yet (deferred until parametric
 *   transforms / ownership policy for Transform2D is decided). Callers keep
 *   the transform they optimized separately.
 *
 * This type is a pure value; it does not throw. Aligns with hybrid error
 * handling (ADR 007): structured results for multi-outcome registration APIs.
 */
struct RegistrationResult {
    /** True if the run finished successfully (optimizer/metric accepted). */
    bool succeeded = false;

    /** Error classification; Ok when succeeded. */
    ErrorCode code = ErrorCode::Ok;

    /** Final metric value (cost or similarity, per the metric used). */
    double finalMetric = 0.0;

    /** Number of optimizer iterations performed (0 if not applicable). */
    int iterations = 0;

    /** Optional human-readable detail (empty if none). */
    std::string message{};

    /**
     * @return A successful result with the given metric and iteration count.
     */
    [[nodiscard]] static RegistrationResult success(double finalMetric,
                                                    int iterations = 0) {
        RegistrationResult r;
        r.succeeded = true;
        r.code = ErrorCode::Ok;
        r.finalMetric = finalMetric;
        r.iterations = iterations;
        return r;
    }

    /**
     * @return A failed result with the given code and optional message.
     */
    [[nodiscard]] static RegistrationResult failure(ErrorCode code,
                                                    std::string message = {}) {
        RegistrationResult r;
        r.succeeded = false;
        r.code = code;
        r.message = std::move(message);
        return r;
    }
};

} // namespace ir
