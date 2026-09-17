# Task: Point2D

**Status**: in progress (TDD — tests first on branch `feature/point2d-tdd-tests`)  
**Component**: core

## Goal
Define a simple, value-semantic 2D point type.

## Acceptance criteria
- Header: `include/ir/core/Point2D.hpp`
- Uses `#pragma once`
- Lives in namespace `ir`
- Provides at least:
  - Construction from x, y
  - Accessors `x()` / `y()` (or equivalent)
  - Basic arithmetic that makes sense for points/vectors
- Preferably thin wrapper around or interoperable with Eigen
- No dynamic allocation
- Unit tests (Catch2) covering construction and basic operations

## TDD notes
- Tests live in `tests/core/test_point2d.cpp` (written first; implementation follows).
- Expected public surface exercised by tests:
  - Default ctor → (0, 0)
  - `Point2D(x, y)`, `x()`, `y()`
  - `+`, `-`, `+=`, `-=`, scalar `*` (both orders), unary `-`
  - `==`, `!=`
  - Trivially copyable / nothrow constructible value type
  - `toEigen()` / `fromEigen()` for `Eigen::Vector2d`

## Notes
Keep it lightweight. This will be used everywhere (transforms, metrics, sampling, etc.).
