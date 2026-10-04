# Task: Rigid2D transform

**Status**: done (branch `feature/rigid2d`)  
**Component**: transform  
**Priority**: High  
**Depends on**: Transform2D, Translation2D (done)

## Goal
Rigid motion in 2D (SE(2)): rotation about the physical origin, then translation.

## Design
- `p' = R(theta) * p + t` (counter-clockwise radians)
- Always invertible; inverse object deferred to Expected

## Acceptance criteria
- [x] `include/ir/transform/Rigid2D.hpp`
- [x] `angle()`, `translation()`
- [x] Catch2 tests
- [x] Implemented note
