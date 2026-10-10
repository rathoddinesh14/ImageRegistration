# Task: LinearInterpolator2D (bilinear)

**Status**: done (branch `feature/linear-interpolator2d`)  
**Component**: interpolator  
**Priority**: Medium  

## Design
- Bilinear weights on unit square in continuous index space
- BoundsPolicy Constant / Clamp via shared `sampleAt()`

## Acceptance criteria
- [x] `LinearInterpolator2D.hpp`
- [x] Analytic Catch2 cases + OOB
- [x] Shared OOB helper with nearest-neighbor
