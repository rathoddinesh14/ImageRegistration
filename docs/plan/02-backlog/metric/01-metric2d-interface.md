# Task: Metric2D pure interface

**Status**: todo  
**Component**: metric  
**Priority**: High  
**Depends on**: Image2D

## Goal
Pure abstraction for similarity/dissimilarity between two images (same geometry assumed for v0.1, or documented requirement).

## Draft API
- `double evaluate(const Image2D& fixed, const Image2D& moving) const`
- Direction: higher-is-better vs lower-is-better documented (`isMinimize()` or metric kind)

## Acceptance criteria
- [ ] `include/ir/metric/Metric2D.hpp`
- [ ] Docs: geometry requirements, empty overlap policy (assert vs Expected later)
- [ ] Tests via stub
