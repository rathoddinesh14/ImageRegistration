# Task: ImageResampler2D concrete

**Status**: done (branch `feature/image-resampler2d`)  
**Component**: resampler  
**Priority**: High  

## Design
- Implements fixed→moving physical pull pipeline
- Composes geometry + Transform2D + Interpolator2D

## Acceptance criteria
- [x] `include/ir/resampler/ImageResampler2D.hpp`
- [x] Catch2: identity + integer translation with NN
