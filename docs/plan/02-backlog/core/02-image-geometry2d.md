# Task: ImageGeometry2D

**Status**: todo  
**Component**: core

## Goal
Represent the geometric properties of a 2D image (origin, spacing, size, orientation).

## Acceptance criteria
- Header: `include/ir/core/ImageGeometry2D.hpp`
- Pure data + query methods (no pixel data)
- Able to convert between index space and physical space
- Clear, documented coordinate conventions
- Unit tests for round-trip index ↔ physical conversions

## Notes
Geometry should be independent of the actual pixel buffer so that the same geometry can later be reused for 3D and time-series concepts.
