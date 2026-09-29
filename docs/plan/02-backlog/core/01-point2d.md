# Task: Point2D

**Status**: done (branch `feature/point2d-tdd-tests`)  
**Component**: core

## Goal
Define a simple, value-semantic 2D point type.

## Acceptance criteria
- [x] Header: `include/ir/core/Point2D.hpp`
- [x] Uses `#pragma once`
- [x] Lives in namespace `ir`
- [x] Construction from x, y; accessors `x()` / `y()`
- [x] Basic arithmetic (`+`, `-`, `+=`, `-=`, scalar `*`, unary `-`)
- [x] Eigen interop (`toEigen` / `fromEigen`)
- [x] No dynamic allocation (trivially copyable value type)
- [x] Unit tests (Catch2) covering construction and basic operations

## Notes
Keep it lightweight. This will be used everywhere (transforms, metrics, sampling, etc.).
