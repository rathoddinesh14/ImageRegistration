# Task: Transform2D (pure interface)

**Status**: done (branch `feature/transform2d-interface-tdd`)  
**Component**: transform

## Goal
Define the pure abstract interface for 2D spatial transforms.

## Locked design
- Pure abstract class `ir::Transform2D`
- `Point2D transformPoint(const Point2D&) const` — pure virtual
- `bool isInvertible() const noexcept` — pure virtual
- Virtual destructor; protected special members for derived types
- Inverse object API deferred until `ir::Expected`
- No production concrete transforms in this task

## Acceptance criteria
- [x] Header: `include/ir/transform/Transform2D.hpp`
- [x] Unit tests: `tests/transform/test_transform2d.cpp`
- [x] Documented ownership and lifetime expectations
- [x] No concrete production implementations in this task
