#pragma once

#include <ir/core/Point2D.hpp>

namespace ir {

/**
 * Pure abstract interface for 2D spatial transforms.
 *
 * Maps points in continuous physical (or parameter) space. Concrete types
 * implement transformPoint; metrics, resamplers, and registration depend on
 * this abstraction rather than specific motion models.
 *
 * Ownership and lifetime:
 * - Transform2D is intended for polymorphic use (base pointers/references).
 * - Prefer unique_ptr<Transform2D> (or shared_ptr when shared ownership is
 *   required) for heap-allocated concrete transforms.
 * - The base has a virtual destructor so deletion through a base pointer is
 *   well-defined.
 *
 * Invertibility:
 * - isInvertible() reports whether an inverse is conceptually available.
 * - Materializing an inverse transform is deferred until ir::Expected (or
 *   equivalent) is available for recoverable failure (e.g. singular maps).
 */
class Transform2D {
public:
    virtual ~Transform2D() = default;

    /**
     * Apply this transform to a point.
     * @param p Input point (typically physical coordinates).
     * @return Transformed point.
     */
    [[nodiscard]] virtual Point2D transformPoint(const Point2D& p) const = 0;

    /**
     * Report whether this transform is invertible.
     * @return True if an inverse is expected to exist; false otherwise.
     */
    [[nodiscard]] virtual bool isInvertible() const noexcept = 0;

protected:
    Transform2D() = default;
    Transform2D(const Transform2D&) = default;
    Transform2D& operator=(const Transform2D&) = default;
    Transform2D(Transform2D&&) = default;
    Transform2D& operator=(Transform2D&&) = default;
};

} // namespace ir
