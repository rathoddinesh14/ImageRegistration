# Implemented: ImageGeometry2D

**Status**: done (branch `feature/image-geometry2d-tdd-tests`)  
**Date**: 2026-10-02

## What was added
- `include/ir/core/ImageGeometry2D.hpp` — size, spacing, origin, 2x2 direction
- `tests/core/test_image_geometry2d.cpp` — TDD tests (written first)
- Coding standards: prefer `/** */` for multi-line public API docs

## Mapping
`physical = origin + direction * (i * sx, j * sy)` (pixel-center origin).
