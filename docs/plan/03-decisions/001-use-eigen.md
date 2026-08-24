# ADR 001 — Use Eigen for linear algebra

**Status**: Accepted  
**Date**: 2026-08-24

## Decision
Eigen is the only required third-party dependency for linear algebra and small dense matrix operations.

## Rationale
- Header-only, excellent performance for the sizes typical in image registration
- Widely used and well-maintained
- Clean integration with modern C++
- Sufficient for 2D/3D transforms, Jacobians, etc.

## Consequences
- Core library depends on Eigen
- No custom matrix/vector types needed for the first versions
