# Task: Image2D

**Status**: todo  
**Component**: core

## Goal
Define the core 2D image type that owns (or views) pixel data + geometry.

## Acceptance criteria
- Header: `include/ir/core/Image2D.hpp`
- Combines pixel storage with `ImageGeometry2D`
- Clear ownership semantics (owning vs non-owning if both are supported)
- Access to pixel values by index
- No I/O code inside this class
- Unit tests for construction, size queries, and basic pixel access

## Notes
Decision still open: Eigen matrix backend vs raw buffer. Prefer the simplest correct design first.
