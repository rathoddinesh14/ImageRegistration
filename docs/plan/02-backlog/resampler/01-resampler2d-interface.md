# Task: Resampler2D pure interface

**Status**: done (branch `feature/resampler2d-interface`)  
**Component**: resampler  
**Priority**: High  

## Design locked
- `resample(moving, fixedGeometry, transform, interpolator) → Image2D`
- Transform maps **fixed physical → moving physical**
- No production concrete resampler in this task

## Acceptance criteria
- [x] `include/ir/resampler/Resampler2D.hpp`
- [x] Transform direction documented
- [x] Stub-based Catch2 tests
