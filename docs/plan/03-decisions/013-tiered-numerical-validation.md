# ADR 013 — Numerical validation uses tiered, scale-aware tolerances

- **Status:** Accepted
- **Date:** 2026-11-18
- **Backlog:** DOC-07
- **Related:** DOC-06; `docs/architecture/reconstruction-testing-strategy.md`

## Context

Reconstruction combines exact structural contracts with floating-point computation, discretized physical models, and approximate algorithms. A single global epsilon would either be too strict for end-to-end reconstruction or too loose for simple operator tests. Increasing tolerances must not conceal a discretization or implementation defect.

The testing strategy already defines validation levels T0–T4, double precision as the baseline, deterministic fixtures, and complementary image metrics.

## Decision

Use the existing tiered T0–T4 validation model and require scale-aware comparisons for approximate numerical results.

- **T0 — Structural:** Exact checks for dimensions, indexing, storage order, metadata presence, and invariants.
- **T1 — Analytical:** Compare simple geometries with independently derived expectations; use tolerances appropriate to the stated discrete model.
- **T2 — Operator consistency:** Verify adjointness using both absolute residual and normalized discrepancy.
- **T3 — End-to-end:** Evaluate synthetic reconstruction using documented RMSE/NRMSE, PSNR, and SSIM definitions and a justified acceptance profile.
- **T4 — Independent reference:** Compare against an implementation or fixture that does not reuse the code under test.
- Use both absolute and relative tolerances where magnitude varies; define a safe denominator for near-zero comparisons.
- Keep double precision as the initial numerical baseline.
- Fix random seeds and record geometry, sampling, normalization, and algorithm configuration with reference results.
- Keep performance benchmarks separate from correctness gates until stable baselines exist.
- A tolerance override must state the affected test, reason, expected error source, and why it does not mask a defect.

Do not create a second tolerance framework in production APIs. Tolerance profiles belong in test support and validation documentation.

## Alternatives considered

1. **One global epsilon.** Rejected because structural, analytical, adjointness, and image-quality tests measure different properties and error sources.
2. **Absolute tolerance only.** Rejected because it behaves poorly across different numerical scales.
3. **Relative tolerance only.** Rejected because comparisons near zero are unstable or undefined without an absolute safeguard.
4. **Visual acceptance.** Rejected because plausible appearance does not establish mathematical or numerical correctness.

## Consequences

- Tests state which validation tier and error source they cover.
- Tolerances are reviewable and traceable rather than selected merely to make a test pass.
- Numerical acceptance criteria remain distinct from algorithmic and discretization choices.
- Future single precision, GPU, or 3D profiles require evidence and explicit extensions; they are not implied by this baseline.

## Related decisions

ADR 012 (operator contract), ADR 001 (Eigen), and ADR 002 (Catch2).
