# Task: Resampler2D pure interface

**Status**: todo  
**Component**: resampler  
**Priority**: High  
**Depends on**: Image2D, Transform2D, Interpolator2D

## Goal
Resample a moving image into the geometry of a fixed image under a physical-space transform.

## Draft responsibility
For each fixed pixel center → physical → transform → moving continuous index → interpolate.

## Acceptance criteria
- [ ] `include/ir/resampler/Resampler2D.hpp` (or similar name)
- [ ] Document transform direction (moving←fixed vs fixed←moving) in header
- [ ] Stub-based interface tests
