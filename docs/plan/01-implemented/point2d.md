# Implemented: Point2D

**Status**: done (branch `feature/point2d-tdd-tests`)  
**Date**: 2026-09-29

## What was added
- `include/ir/core/Point2D.hpp` — value type with arithmetic and Eigen interop
- `tests/core/test_point2d.cpp` — TDD tests (written first, then implementation)

## API summary
- Construction: default origin, `(x, y)`
- Accessors: `x()`, `y()`
- Operators: `+=`, `-=`, `+`, `-`, unary `-`, `*` (both scalar orders), `==`, `!=`
- Eigen: `toEigen()`, `fromEigen()`
