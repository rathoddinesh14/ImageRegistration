# Task: Registration2D / method interface

**Status**: todo  
**Component**: registration  
**Priority**: Medium  
**Depends on**: Metric, Resampler, Optimizer, RegistrationResult

## Goal
Pure (or thin) interface for running 2D image registration: fixed + moving + transform → result.

## Acceptance criteria
- [ ] Header under `include/ir/registration/`
- [ ] Inputs/outputs documented (ownership of transform)
- [ ] No full algorithm required in this task
