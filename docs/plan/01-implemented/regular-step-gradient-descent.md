# Implemented: RegularStepGradientDescent

**Status**: done (branch `feature/regular-step-gd`)  
**Date**: 2026-10-17

Finite-difference regular-step GD for demos and convex toy costs.

## Limitations
- Value-only finite-difference gradients (no analytic derivatives)
- Not Ceres / production ITK optimizer quality
- Simple grow/shrink step; sensitive to learning rate and epsilon
- Not intended for large or highly non-convex registration problems alone
