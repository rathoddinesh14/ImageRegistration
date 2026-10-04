# Task: Affine2D transform

**Status**: todo  
**Component**: transform  
**Priority**: Medium  
**Depends on**: Transform2D, preferably Rigid2D

## Goal
2D affine map: linear `2×2` part + translation (`p' = A p + t`).

## Acceptance criteria
- [ ] `include/ir/transform/Affine2D.hpp` (replace empty placeholder)
- [ ] Construct from matrix + translation (Eigen `Matrix2d` + `Point2D`)
- [ ] `isInvertible()` based on non-singular linear part (or always report and defer inverse)
- [ ] Catch2 tests: identity, pure translation, known linear map
- [ ] Docs: relationship to Rigid2D (Rigid is special case)

## Out of scope
- Full projective transforms
- Parameterization for optimizers (can be a follow-up)
