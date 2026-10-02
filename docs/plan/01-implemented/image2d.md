# Implemented: Image2D

**Status**: done (branch `feature/image2d-tdd-tests`)  
**Date**: 2026-10-02

## What was added
- `include/ir/core/Image2D.hpp` — owning double image + geometry
- `tests/core/test_image2d.cpp` — TDD tests (written first)

## Layout
Row-major: `linearIndex = j * width + i`.
