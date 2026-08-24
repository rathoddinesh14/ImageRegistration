# Task: Document coordinate & indexing conventions

**Status**: todo  
**Component**: core / documentation

## Goal
Write a short, authoritative document that defines:
- Index space vs physical space
- Origin convention
- Spacing
- Orientation / direction matrix (even if initially identity for 2D)
- Pixel-center vs corner conventions (if relevant)

## Acceptance criteria
- File lives under `docs/architecture/` or `docs/plan/`
- Referenced by `ImageGeometry2D` and `Image2D` headers
- Clear enough that future 3D and time-series code can follow the same rules
