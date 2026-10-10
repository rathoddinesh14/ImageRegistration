# Task: MeanSquaresMetric2D (MSE)

**Status**: done (branch `feature/mean-squares-metric2d`)  
**Component**: metric  
**Priority**: High  

## Design
- MSE over all fixed lattice pixels; same width/height required (assert)
- `isMinimize()` always true

## Acceptance criteria
- [x] `MeanSquaresMetric2D.hpp`
- [x] Catch2: identical → 0; constant difference; polymorphism
