# Task: ImageResampler2D concrete

**Status**: todo  
**Component**: resampler  
**Priority**: High  
**Depends on**: Resampler2D interface, Interpolator2D, Translation2D (for tests)

## Goal
Default resampler implementation composing geometry mapping + interpolator + transform.

## Acceptance criteria
- [ ] Concrete type under `include/ir/resampler/`
- [ ] Catch2: translate by integer pixels with NN matches shifted buffer
- [ ] Optional PNG sample in CI later (not required for this task)
