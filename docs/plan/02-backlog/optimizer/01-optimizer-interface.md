# Task: Optimizer pure interface

**Status**: todo  
**Component**: optimizer  
**Priority**: Medium  
**Depends on**: parametric transform interface (recommended)

## Goal
Abstract iterative parameter updates for registration (no Ceres dependency in core).

## Draft API
- Configure max iterations, step size, tolerance
- `optimize(cost function or registration callback)` — keep minimal and document

## Acceptance criteria
- [ ] Header under `include/ir/optimizer/`
- [ ] Documented lifecycle; no concrete algorithm required in this task
