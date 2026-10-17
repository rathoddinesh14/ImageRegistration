# Task: Optimizer pure interface

**Status**: done (branch `feature/optimizer-interface`)  
**Component**: optimizer  
**Priority**: Medium  

## Design locked
- `CostFunction`: numberOfParameters(), value(const double*)
- `Optimizer::optimize(cost, parameters) → RegistrationResult`
- Parameters caller-owned in/out; config on concrete types
- No production algorithm in this task

## Acceptance criteria
- [x] Headers under `include/ir/optimizer/`
- [x] Documented lifecycle
- [x] Stub Catch2 tests
