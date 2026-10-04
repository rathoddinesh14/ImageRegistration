# Implemented: Interpolator2D interface

**Status**: done (branch `feature/interpolator2d-interface`)  
**Date**: 2026-10-04

## API
`double evaluate(const Image2D&, const Point2D& continuousIndex) const`
Continuous index space; OOB policy documented on the interface (default recommendation: 0.0).
