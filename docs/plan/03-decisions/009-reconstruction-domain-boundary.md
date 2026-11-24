# ADR 009 — CT reconstruction is a peer domain

- **Status:** Accepted
- **Date:** 2026-11-18
- **Backlog:** DOC-07
- **Related:** DOC-01, DOC-02; `docs/architecture/reconstruction-domain.md`

## Context

ImageRegistration began as an image-registration library, but the repository now also contains a scoped 2D parallel-beam CT reconstruction initiative. Registration estimates spatial relationships between images; reconstruction estimates a spatial image from projection measurements. They share useful primitives, but their domain concepts and algorithms are not interchangeable.

The initial reconstruction workflow is synthetic-first: phantom → parallel-beam projections → filtered backprojection → numerical validation. The core must remain independent of scanner I/O, DICOM, visualization, and GUI frameworks.

## Decision

Treat CT reconstruction as an independent, peer domain within the existing ImageRegistration repository.

- Keep reconstruction-specific public APIs under `ir/reconstruction/`.
- Reuse `ir::Image2D`, `ir::ImageGeometry2D`, and mathematical primitives only where their semantics match.
- Keep acquisition geometry, projection measurements, and reconstruction orchestration separate from generic image and registration types.
- Preserve one repository, build, test, and release workflow; do not create a nested Git submodule.
- Keep registration and reconstruction independently buildable and testable. Neither domain may depend on the other's algorithms.
- Keep I/O and application adapters at the boundary; numerical algorithms consume validated domain contracts.

## Alternatives considered

1. **Create a separate repository.** This gives stronger repository isolation but duplicates build, CI, contribution, and shared-core maintenance for a closely related library.
2. **Make reconstruction a submodule.** This introduces version pinning and cross-repository coordination without a current need for independent release cadence.
3. **Fold reconstruction into registration abstractions.** This appears to maximize reuse but conflates distinct mathematical domains and risks CT-specific metadata leaking into generic image types.

## Consequences

- The repository name remains broader than its original registration-only scope; module boundaries must therefore be explicit.
- Shared core types stay domain-neutral.
- Reconstruction can evolve its own geometry, data, algorithms, and tests without changing registration behavior.
- Future 3D, fan-beam, or real-data work still requires separately scoped design decisions; this ADR does not authorize speculative generic frameworks.

## Related decisions

ADR 004 (pure interfaces first), ADR 001 (Eigen), and ADR 002 (Catch2).
