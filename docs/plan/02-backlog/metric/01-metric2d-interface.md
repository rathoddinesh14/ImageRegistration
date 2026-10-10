# Task: Metric2D pure interface

**Status**: done (branch `feature/metric2d-interface`)  
**Component**: metric  
**Priority**: High  

## Design locked
- `evaluate(fixed, moving) → double`
- `isMinimize() → bool` (cost vs similarity)
- Same size expected in v0.1; Expected deferred for mismatch/empty overlap
- No production concrete metric in this task

## Acceptance criteria
- [x] `include/ir/metric/Metric2D.hpp`
- [x] Catch2 tests via stubs
