# ADR 011 — Acquisition geometry is distinct from image-grid geometry

- **Status:** Accepted
- **Date:** 2026-11-18
- **Backlog:** DOC-07
- **Related:** DOC-04; `docs/architecture/ct-acquisition-geometry.md`; `docs/architecture/coordinate-conventions.md`

## Context

An image grid describes where image samples live. A CT acquisition describes which rays produced projection samples. They are related through physical coordinates, but are not the same object and have different ownership and evolution requirements.

The initial geometry is 2D parallel beam. The project has already documented a right-handed physical `(x,y)` frame, radians, counterclockwise positive angles from `+x`, and a centered detector convention whose center lies between the central samples when the detector count is even.

## Decision

Represent acquisition geometry as a reconstruction-domain contract separate from `ir::ImageGeometry2D`.

For the initial parallel-beam geometry:

- Preserve view angles in acquisition order, in radians.
- Define the projection normal as `n(θ) = (cos θ, sin θ)` and ray direction as `u(θ) = (-sin θ, cos θ)`.
- Define each ray by `n(θ) · (r - c) = s`, where `c` is the acquisition reference center.
- Define detector coordinates explicitly. For a centered detector, `s(d) = (d - (D - 1)/2) Δs`.
- Require finite, strictly positive detector spacing and consistent physical units.
- Keep detector samples, view ordering, detector offset, and geometry metadata mutually consistent.
- Do not silently sort angles, reverse detector order, transpose data, or apply an offset twice.

Image-grid geometry remains responsible for image dimensions, spacing, origin, and direction. Acquisition geometry does not own the sinogram buffer or perform I/O.

## Alternatives considered

1. **Add CT acquisition fields to `ImageGeometry2D`.** Rejected because it makes a generic spatial image type scanner-specific.
2. **Infer acquisition geometry from sinogram dimensions.** Rejected because dimensions do not encode angles, physical spacing, reference center, or coordinate conventions.
3. **Use an unqualified angle/detector tuple at each algorithm call.** Rejected because it duplicates validation and allows inconsistent parameters across pipeline stages.

## Consequences

- Geometry compatibility can be validated before numerical operations.
- Forward projection, filtering, and backprojection can share one explicit acquisition description.
- Future fan-beam and cone-beam geometries require their own separately specified contracts; this ADR does not generalize the initial API prematurely.
- Existing registration coordinate conventions remain unchanged.

## Related decisions

ADR 009 (domain boundary), ADR 010 (projection semantics), and ADR 012 (operator contract).
