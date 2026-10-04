# Task: Regular step / simple gradient descent optimizer

**Status**: todo  
**Component**: optimizer  
**Priority**: Medium  
**Depends on**: Optimizer interface, Metric + Resampler (for cost)

## Goal
Minimal derivative-free or finite-difference step optimizer sufficient for Translation2D demos.

## Acceptance criteria
- [ ] Concrete optimizer implementing the interface
- [ ] Catch2: synthetic 1D/2D cost decreases on a convex toy problem
- [ ] Document limitations (not production Ceres)

## Out of scope
- Linking Ceres (optional later backlog)
