# Task: Optimizable transform parameters (interface)

**Status**: todo  
**Component**: transform  
**Priority**: Medium (before gradient-based registration)  
**Depends on**: Transform2D; useful with Rigid2D / Affine2D

## Goal
Define how transforms expose a flat parameter vector for optimizers (Ceres/ITK-style), without tying core to a specific optimizer.

## Draft API (refine at implementation)
- `int numberOfParameters() const`
- `void setParameters(span or vector of double)`
- `void getParameters(...) const`
- Optional: only on a sub-interface `ParametricTransform2D` so pure geometric transforms stay simple

## Acceptance criteria
- [ ] Documented interface header under `include/ir/transform/`
- [ ] At least one concrete type implements it (e.g. Rigid2D) in this or a linked task
- [ ] Catch2 tests for set/get round-trip
- [ ] ADR or short design note if choosing sub-interface vs methods on base

## Out of scope
- Actual optimizer implementation
