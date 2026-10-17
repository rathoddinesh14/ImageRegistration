# ADR 008 — ParametricTransform2D sub-interface

**Status**: Accepted  
**Date**: 2026-10-17  
**Context**: Optimizers need a flat `double*` parameter vector; not every
Transform2D must be optimizable via that layout.

## Decision
- Introduce `ParametricTransform2D` **deriving from** `Transform2D`.
- Methods: `numberOfParameters()`, `getParameters(double*)`, `setParameters(const double*)`.
- Do **not** put parameters on the base `Transform2D` (keeps pure geometric use simple).
- `Translation2D` and `Rigid2D` implement the sub-interface.
- Layout is documented per type (Translation: `[tx,ty]`; Rigid: `[theta,tx,ty]`).

## Consequences
- Registration/optimizer code can `dynamic_cast` or accept `ParametricTransform2D&`.
- Affine and others will implement the same contract when added.
