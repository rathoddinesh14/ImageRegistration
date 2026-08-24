# Task: Point2D

**Status**: todo  
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

## Notes
Keep it lightweight. This will be used everywhere (transforms, metrics, sampling, etc.).
