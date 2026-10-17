# Task: Registration2D / method interface

**Status**: done (branch `feature/registration2d-interface`)  
**Component**: registration  
**Priority**: Medium  

## Design locked
- `run(fixed, moving, transform) → RegistrationResult`
- Transform is caller-owned in/out (fixed physical → moving physical)
- No production algorithm in this task

## Acceptance criteria
- [x] `include/ir/registration/Registration2D.hpp`
- [x] Inputs/outputs documented
- [x] Stub Catch2 tests
