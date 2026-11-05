# Discrete Projection Operators: Forward Model and Adjoint Contract

- Backlog ID: DOC-05
- Related issue: #37
- Dependencies: #35 (DOC-03), #36 (DOC-04)
- Status: Proposed operator contract
- Scope: 2D parallel-beam CT, synthetic data

## 1. Purpose

This document defines the mathematical and software contract for discrete forward projection and its mathematical adjoint in the initial CT reconstruction workflow.

It connects the continuous acquisition geometry described in `docs/architecture/ct-acquisition-geometry.md` to finite-dimensional image and sinogram arrays. This specification describes intended behavior rather than an implementation that already exists.

## 2. Domains and Shapes

Let the image grid have width `W` and height `H`. Let the sinogram have `V` views and `D` detector samples per view.

- Image domain: `X`, containing images with shape `W × H`
- Projection domain: `Y`, containing sinograms with logical shape `[V, D]`
- Image storage: row-major, with `imageIndex(i, j) = j * W + i`
- Projection storage: row-major, with `projectionIndex(v, d) = v * D + d`
- Geometry: the supplied image grid and acquisition geometry determine the physical position of image samples and the ray corresponding to each `(v, d)`

Operators must preserve the supplied view order, detector order, and image geometry.

Example:

A `2 × 3` image has 6 voxels/pixels. If the grid is `[[x0, x1, x2], [x3, x4, x5]]`, then the linear index of `x4` is `imageIndex(1, 1) = 1 * 3 + 1 = 4` in row-major layout. For a single-view sinogram with 4 detector bins, the detector sample at index `d = 3` is stored at `projectionIndex(0, 3) = 0 * 4 + 3 = 3`.

## 3. Discrete Forward Operator

The forward operator maps an image to its simulated projection measurements:

`A : X -> Y`

`y = A x`

For a ray–pixel intersection discretization, each matrix coefficient `A[(v, d), (i, j)]` represents the physical length of intersection between ray `(v, d)` and pixel `(i, j)`.

The projection is therefore:

`(A x)[v, d] = Σ(i, j) A[(v, d), (i, j)] x[i, j]`

This contract assumes a piecewise-constant value within each pixel. Other discretizations, such as interpolation-based sampling, must explicitly document their sampling, interpolation, and boundary rules.

The implementation must not assume that image pixels are axis-aligned when the supplied image direction matrix specifies a rotated or reflected grid.

Example:

Consider a `2 × 2` image with values

`x = [x00, x01; x10, x11] = [1, 2; 3, 4]`

and a single ray that intersects the four pixels with lengths

`A[(0, 0), :] = [0.5, 0.25, 0.25, 0.0]`

Then the forward projection at detector sample `(0, 0)` is

`(A x)[0, 0] = 0.5 * 1 + 0.25 * 2 + 0.25 * 3 + 0.0 * 4 = 2.25`

This shows the core meaning of the operator: each pixel contributes a weighted amount to the measured ray integral.

## 4. Mathematical Adjoint

For the initial contract, define Euclidean inner products over the stored arrays.

Image space:

`⟨x₁, x₂⟩X = Σ(i, j) x₁[i, j] x₂[i, j]`

Projection space:

`⟨y₁, y₂⟩Y = Σ(v, d) y₁[v, d] y₂[v, d]`

Under these inner products, the mathematical adjoint is the transpose of the discrete forward operator:

`A* = Aᵀ`

It must satisfy:

`⟨A x, y⟩Y = ⟨x, A* y⟩X`

in exact arithmetic.

Detector spacing, pixel area, angular quadrature, and other physical integration weights are not implicitly applied by these Euclidean inner products.

Example:

Suppose

`x = [1, 2, 3]` and `y = [4, 5, 6]`

Then the Euclidean inner product is

`⟨x, y⟩ = 1*4 + 2*5 + 3*6 = 32`

If `A` is the matrix

`A = [[1, 0, 0], [1, 1, 0], [0, 1, 1]]`,

then `A x = [1, 3, 5]` and

`⟨A x, y⟩ = 1*4 + 3*5 + 5*6 = 49`

The adjoint `Aᵀ` satisfies

`Aᵀ y = [4 + 5, 5 + 6, 6] = [9, 11, 6]`

and

`⟨x, Aᵀ y⟩ = 1*9 + 2*11 + 3*6 = 49`

This is the discrete adjoint identity in action.

### 4.1 Weighted Inner Products

If weighted inner products are introduced in the future, they must be explicitly documented. For positive-definite weight matrices `WX` and `WY`, the corresponding adjoint is:

`A* = WX⁻¹ Aᵀ WY`

This is a different contract from the unweighted Euclidean adjoint. Weighting and normalization must never be hidden inside an implementation.

Example:

Let

`x = [1, 2]`, `y = [3, 4]`, `WX = diag(1, 2)`, `WY = diag(2, 1)`

Then the weighted inner products are

`⟨x₁, x₂⟩WX = x₁ᵀ WX x₂ = x₁ᵀ [x₂0, 2x₂1]`

and

`⟨y₁, y₂⟩WY = y₁ᵀ WY y₂`.

In this case, the adjoint is no longer simply `Aᵀ`; it becomes `WX⁻¹ Aᵀ WY`. This matters because the weighting changes the notion of “orthogonality” and the corresponding transpose relationship.

## 5. Mathematical Adjoint Versus FBP Backprojection

A backprojection operation used in filtered backprojection (FBP) is not automatically the mathematical adjoint of a discrete forward projector.

- Mathematical adjoint: must satisfy the declared discrete inner-product identity.
- FBP backprojection: may include interpolation, angular quadrature, normalization, or other factors required by the reconstruction formula.
- Naming: an implementation must not claim to implement an adjoint unless it satisfies and tests the stated adjoint contract.

Iterative reconstruction algorithms that rely on adjointness must use a compatible forward/adjoint pair.

Example:

If the forward model is

`A = [[1, 0], [1, 1], [0, 1]]`,

then its transpose is

`Aᵀ = [[1, 1, 0], [0, 1, 1]]`.

This matrix is the mathematical adjoint of the forward operator in the Euclidean inner-product sense. By contrast, a typical FBP backprojector may apply an additional angular weighting or interpolation to produce a reconstruction image, not merely the transpose of the projection matrix. That extra processing makes it a reconstruction operator, not necessarily the exact adjoint of the discrete projector.

## 6. Geometry and Boundary Assumptions

A concrete implementation must document:

- the ray model and discretization method
- how detector coordinates are derived from acquisition geometry
- whether coefficients represent physical intersection lengths or another approximation
- how pixel and ray boundary contacts are handled
- how image support outside the grid is treated
- how spacing and direction affect calculations in physical space
- whether detector or angular weights are part of the operator or a separate algorithm stage

The initial scope is ideal 2D parallel-beam line-integral projection. Raw photon-count simulation, scanner calibration, fan-beam geometry, and cone-beam geometry are outside this contract.

Example:

For a single detector bin and a `2 × 2` grid, suppose a ray passes through the top-left and bottom-right pixels with lengths `0.5` and `0.5`. Then a pixel outside the grid should not be implicitly included unless the implementation explicitly defines a boundary rule such as zero-padding or clamping. This is why the contract requires the boundary policy to be documented: different choices can change the resulting projection matrix even for the same geometry.

## 7. Adjointness Test

For a deterministic test image `x` and sinogram `y`, compute:

`lhs = ⟨A x, y⟩Y`

`rhs = ⟨x, A* y⟩X`

A scale-aware discrepancy is:

`err = |lhs - rhs| / max(|lhs|, |rhs|, ε)`

Here, `ε` is a small positive safeguard appropriate to the numeric type and test scale. The test must document a justified tolerance based on floating-point precision and accumulation behavior.

Tests should:

- use deterministic, nontrivial image and sinogram values, including mixed signs
- include small geometries that make failures reproducible
- check output shapes and geometry
- avoid relying only on zero-valued or symmetric inputs
- test invalid inputs separately from numerical adjointness
- avoid using visual similarity as evidence of adjointness

Adjointness is necessary for an adjoint claim, but it does not prove that the forward model is physically accurate. Independent known-geometry tests are also required.

Example:

Let

`x = [1, -2, 3]` and `y = [4, -1, 2]`.

For a forward operator `A`, compute `lhs = ⟨A x, y⟩` and `rhs = ⟨x, Aᵀ y⟩`. If floating-point arithmetic is used, a small mismatch may appear because sums are accumulated in a different order. The discrepancy should be checked against a tolerance such as `err = |lhs - rhs| / max(|lhs|, |rhs|, ε)`, with `ε` chosen to avoid division by zero. This makes the test numerically meaningful even when the values are small.

## 8. Test Strategy

1. Contract tests: shapes, indexing, geometry compatibility, and invalid inputs.
2. Known-geometry tests: simple configurations with predictable ray–pixel intersection lengths or projections.
3. Adjointness tests: verify the discrete inner-product identity.
4. Integration tests: exercise the operators with the documented projection data and acquisition geometry.
5. Algorithm tests: test FBP and iterative reconstruction separately; they do not replace projector contract tests.

Follow Red → Green → Refactor: add a focused failing test, implement the smallest behavior that satisfies it, then refactor while preserving existing tests.

## 9. Deferred Decisions

- weighted image/projection inner products
- nonuniform angular quadrature and detector weighting
- alternative discrete projector/adjoint pairs
- fan-beam or cone-beam geometry
- GPU and parallel accumulation strategies
- scanner-specific measurement models and calibration

## 10. Definition of Done

- [ ] Operator domains, codomains, and array shapes are explicit.
- [ ] Discrete ray/pixel contributions are documented.
- [ ] Inner products and weighting conventions are explicit.
- [ ] Mathematical adjoint and FBP backprojection are distinguished.
- [ ] A numerical adjointness test is specified.
- [ ] Geometry, sampling, and boundary assumptions are documented.
- [ ] The contract is consistent with DOC-03 and DOC-04.
