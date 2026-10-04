# Task: NearestNeighborInterpolator2D

**Status**: done (branch `feature/nearest-neighbor-interpolator`)  
**Component**: interpolator  
**Priority**: High  

## Design
- Round continuous index with `std::lround`
- `BoundsPolicy::Constant` (default fill 0) or `Clamp`
- Constant(0) is convenience only; not ideal for metrics long-term

## Acceptance criteria
- [x] Header + BoundsPolicy
- [x] Catch2: centers, rounding, Constant/Clamp OOB, polymorphism
