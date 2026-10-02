# Task: ImageGeometry2D

**Status**: done (branch `feature/image-geometry2d-tdd-tests`)  
**Component**: core

## Goal
Represent the geometric properties of a 2D image (origin, spacing, size, orientation).

## Locked design decisions
- Origin = physical position of the **center** of pixel index `(0, 0)`
- Index size type = `int`; discrete indices are integers
- Empty geometry **not allowed** (width >= 1, height >= 1; precondition / assert)
- Direction = full **2x2** `double` matrix (Eigen::Matrix2d)
- Spacing, origin, physical coordinates use **double** / `Point2D`
- Public docs use `/** */` block style

## Mapping
`physical = origin + direction * (i * sx, j * sy)` for integer index `(i, j)` (pixel center).

## Acceptance criteria
- [x] Header: `include/ir/core/ImageGeometry2D.hpp`
- [x] Pure data + query methods (no pixel data)
- [x] Index space ↔ physical space
- [x] Unit tests: `tests/core/test_image_geometry2d.cpp`

## Notes
Geometry should be independent of the actual pixel buffer so that the same geometry can later be reused for 3D and time-series concepts.
