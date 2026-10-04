# Task: Interpolator2D pure interface

**Status**: done (branch `feature/interpolator2d-interface`)  
**Component**: interpolator  
**Priority**: High  

## Design locked
- Continuous **index** space `(i, j)` via `Point2D`
- `evaluate(const Image2D&, const Point2D&) const`
- v0.1 OOB: concrete types document policy; recommended return `0.0`

## Acceptance criteria
- [x] `include/ir/interpolator/Interpolator2D.hpp`
- [x] Catch2 tests via test-only stub
- [x] No production concrete interpolator in this task
