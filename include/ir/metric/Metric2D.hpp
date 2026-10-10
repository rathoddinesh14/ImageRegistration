#pragma once

#include <ir/core/Image2D.hpp>

namespace ir {

/**
 * Pure abstract interface for a 2D image similarity / dissimilarity metric.
 *
 * evaluate(fixed, moving) returns a scalar score. Whether smaller or larger
 * values are better is reported by isMinimize():
 * - true  — cost-like (e.g. mean squares); optimizers should minimize
 * - false — similarity-like (e.g. NCC); optimizers should maximize
 *
 * Geometry (v0.1):
 * - Callers should pass images with matching size (width/height). Concrete
 *   metrics may assert or document stricter rules (same full geometry).
 * - Empty overlap / size mismatch: assert or document for v0.1; recoverable
 *   signaling via ir::Expected is deferred.
 *
 * Ownership: polymorphic via base pointers/references; virtual destructor.
 */
class Metric2D {
public:
    virtual ~Metric2D() = default;

    /**
     * Compute the metric between fixed and moving images.
     * @param fixed Reference (fixed) image.
     * @param moving Moving image (same size expected in v0.1).
     * @return Scalar metric value.
     */
    [[nodiscard]] virtual double evaluate(const Image2D& fixed,
                                          const Image2D& moving) const = 0;

    /**
     * Report optimization direction.
     * @return True if lower values are better (minimize); false if higher is better.
     */
    [[nodiscard]] virtual bool isMinimize() const noexcept = 0;

protected:
    Metric2D() = default;
    Metric2D(const Metric2D&) = default;
    Metric2D& operator=(const Metric2D&) = default;
    Metric2D(Metric2D&&) = default;
    Metric2D& operator=(Metric2D&&) = default;
};

} // namespace ir
