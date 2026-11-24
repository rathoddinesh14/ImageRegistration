# ADR 012 — Forward projection and mathematical adjoint are a matched pair

- **Status:** Accepted
- **Date:** 2026-11-18
- **Backlog:** DOC-07
- **Related:** DOC-05; `docs/algorithms/projection-operators.md`

## Context

Iterative reconstruction methods rely on a forward operator and its mathematical adjoint. A visually plausible backprojection is not necessarily the adjoint of the discrete forward projector. Confusing the mathematical adjoint with the filtered-backprojection (FBP) reconstruction stage can invalidate optimization algorithms and adjointness tests.

The initial discrete model uses ray–pixel intersection lengths and piecewise-constant pixel values. The declared image and projection inner products are Euclidean sums over stored samples.

## Decision

Specify and test a matched discrete operator pair:

- Forward projection is $A : X \to Y$, mapping image samples to projection samples.
- Under the initial Euclidean inner products, the mathematical adjoint is $A^* = A^\mathsf{T}$.
- The pair must satisfy $\langle Ax, y \rangle_Y = \langle x, A^*y \rangle_X$ up to the documented floating-point tolerance.
- Pixel intersection coefficients represent physical path lengths for the selected discretization.
- Geometry, shape, ordering, boundary behavior, and spacing conventions must be identical and explicit for both operators.
- Do not silently insert detector spacing, pixel-area, angular quadrature, or normalization weights into the Euclidean inner products.
- If weighted inner products are introduced later, document the weight operators and the corresponding adjoint $A^* = W_X^{-1} A^\mathsf{T} W_Y$ as a distinct contract.
- Name an operation `Adjoint` only if it satisfies the declared identity. FBP backprojection remains a separate reconstruction operation and may include filtering, quadrature, interpolation, or normalization.

## Alternatives considered

1. **Call any backprojection the adjoint.** Rejected because the name would promise a property that may not hold.
2. **Use weighted inner products immediately.** Rejected for the baseline because weights add another convention before the unweighted discrete contract is established.
3. **Implement only an FBP backprojector.** Rejected because it does not provide the matched operator pair required by future iterative methods.

## Consequences

- Adjointness becomes an independently testable operator property, not a visual assessment.
- Analytical known-geometry tests remain necessary: adjointness alone cannot prove physical correctness.
- FBP and iterative reconstruction can share compatible geometry and data contracts without conflating their operators.
- Any future discretization change must update both operators and their contract tests together.

## Related decisions

ADR 010 (projection semantics), ADR 011 (geometry), and ADR 013 (numerical validation).
