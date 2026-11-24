# ADR 010 — Projection data carries explicit measurement semantics

- **Status:** Accepted
- **Date:** 2026-11-18
- **Backlog:** DOC-07
- **Related:** DOC-03; `docs/architecture/reconstruction-data-model.md`

## Context

Arrays of identical shape can represent physically different quantities: raw detector signals, corrected transmission, logarithmic line integrals, filtered projections, or reconstructed image values. Treating these as interchangeable can produce mathematically invalid reconstruction while still allowing code to compile.

The first milestone generates idealized line-integral projections directly from a synthetic attenuation-like image. It does not simulate photon counts, apply detector corrections, or ingest scanner files.

## Decision

Projection data must have an explicit, validated semantic identity; an unqualified numeric array is not a sufficient public contract.

- The initial sinogram represents idealized logarithmic line-integral samples, not raw detector counts or transmitted intensities.
- Use logical shape `[view, detector] = [V, D]`, contiguous row-major storage, and `index(v,d) = v * D + d`.
- Preserve the association between each view row and its acquisition angle; never reorder samples independently of per-view metadata.
- Carry or reference the acquisition geometry and document units, scaling, provenance, and supported measurement interpretation.
- Keep filtered projections distinct from unfiltered input projections at the API/algorithm boundary.
- A future conversion from transmission to line integrals must be explicit and validate compatible, finite, strictly positive `I` and `I₀` values before applying `-ln(I/I₀)`.
- Reject unsupported or ambiguous measurement semantics rather than guessing or silently converting them.

## Alternatives considered

1. **One generic `Sinogram` with caller-interpreted values.** Minimal type surface, but physical meaning is implicit and invalid combinations are easy to create.
2. **Treat all projections as line integrals.** Suitable for the synthetic baseline but unsafe for future real acquisition data.
3. **Separate types for every possible scanner signal immediately.** Strongly explicit, but premature while real-data formats and correction pipelines are out of scope.

## Consequences

- Initial implementation can use a focused line-integral representation without pretending to support raw acquisition data.
- Future measurement types must be added through explicit types or tagged semantics and documented conversion steps.
- Array dimensions, storage order, and measurement meaning become testable invariants.
- DICOM CT images must not be treated as projection sinograms; real projection-data ingestion remains deferred.

## Related decisions

ADR 009 (domain boundary), ADR 011 (acquisition geometry), and ADR 012 (operator contract).
