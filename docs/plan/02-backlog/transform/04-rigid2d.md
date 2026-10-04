# Task: Rigid2D transform

**Status**: todo  
**Component**: transform  
**Priority**: High  
**Depends on**: Transform2D, Translation2D (done)

## Goal
Rigid motion in 2D (SE(2)): rotation about origin (or documented center) plus translation.

## Design notes
- Parameters: angle (radians) + translation `Point2D` (exact center convention: document in header; prefer rotate-then-translate about physical origin unless ADR says otherwise)
- `isInvertible()` always `true`
- Inverse object deferred until `ir::Expected` (or provide pure math helpers only)
- Header-only if feasible; use `std::cos` / `std::sin` or Eigen 2D rotation

## Acceptance criteria
- [ ] `include/ir/transform/Rigid2D.hpp` (replace empty placeholder)
- [ ] Accessors: `angle()`, `translation()` (names TBD, camelCase methods)
- [ ] `transformPoint` matches documented formula
- [ ] Catch2 tests: pure translation case, pure rotation of axis points, combined, base polymorphism
- [ ] Short note under `docs/plan/01-implemented/` when done

## Out of scope
- Scaling / shear (see Affine2D)
- Optimizable parameter vector API (separate backlog if needed)
