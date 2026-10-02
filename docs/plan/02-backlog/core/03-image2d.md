# Task: Image2D

**Status**: in progress (TDD — tests first on branch `feature/image2d-tdd-tests`)  
**Component**: core

## Goal
Define the core 2D image type that owns pixel data + geometry.

## Locked design decisions (v0.1)
- Pixel type: `double`
- Storage: owning `std::vector<double>`, **row-major** (`index = j * width + i`)
- Geometry: composed `ImageGeometry2D` (fixed at construction)
- Access: `at(i, j)` and `operator()(i, j)` (const and non-const)
- Helpers: `fill`, `data` / `dataSize`, `containsIndex`, `pixelCount`
- No I/O; no non-owning view type in v0.1

## Acceptance criteria
- [ ] Header: `include/ir/core/Image2D.hpp`
- [ ] Combines pixel storage with `ImageGeometry2D`
- [x] Unit tests: `tests/core/test_image2d.cpp` (written first)

## Notes
Eigen matrix backend deferred; simplest correct owning buffer first.
