# Task: Interpolator2D pure interface

**Status**: todo  
**Component**: interpolator  
**Priority**: High  
**Depends on**: Image2D, Point2D, conventions doc (done)

## Goal
Pure abstraction: evaluate an image at a continuous index (or physical point—**pick one and document**; prefer continuous **index** space for interpolators).

## Draft API
- `double evaluate(const Image2D& image, const Point2D& continuousIndex) const`
- Policy for out-of-bounds (return 0, edge clamp, or signal via Expected later)

## Acceptance criteria
- [ ] `include/ir/interpolator/Interpolator2D.hpp`
- [ ] Documented bounds policy for v0.1
- [ ] Catch2 tests via test-only stub
- [ ] No production concrete interpolator required in this task
