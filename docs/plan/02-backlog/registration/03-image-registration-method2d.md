# Task: ImageRegistrationMethod2D (concrete pipeline)

**Status**: todo  
**Component**: registration  
**Priority**: Medium  
**Depends on**: Registration2D interface, concrete metric/resampler/optimizer

## Goal
Compose metric + resampler + optimizer + transform into an end-to-end 2D registration runnable from tests/examples.

## Acceptance criteria
- [ ] Concrete method type
- [ ] Catch2: recover known Translation2D on synthetic shifted image (within tolerance)
- [ ] Returns `RegistrationResult`
