# Implemented: Transform2D interface

**Status**: done (branch `feature/transform2d-interface-tdd`)  
**Date**: 2026-10-03

## What was added
- `include/ir/transform/Transform2D.hpp` — pure abstract 2D transform
- `tests/transform/test_transform2d.cpp` — contract tests via test-only stubs

## API
- `transformPoint(const Point2D&) const`
- `isInvertible() const noexcept`
- Virtual destructor for polymorphic ownership
