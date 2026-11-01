# Parallel-Beam CT Geometry: Radon Transform and Beer–Lambert Model

- **Backlog ID:** DOC-04
- **Related issue:** #36
- **Dependencies:** #34 (DOC-02), #35 (DOC-03)
- **Status:** Proposed geometry contract
- **Initial scope:** 2D parallel-beam CT
- **Future scope:** Fan-beam and cone-beam geometry, documentation only

---

## 1. Purpose and scope

This document defines the acquisition geometry and measurement model used by the initial 2D parallel-beam CT reconstruction workflow in ImageRegistration.

It specifies how projection angles, detector coordinates, image physical coordinates, and sinogram samples define the 2D Radon transform, and how that line-integral model relates to transmitted intensity through the Beer–Lambert law.

This document defines conventions for future implementations. It does not claim that acquisition-geometry types or projection algorithms have already been implemented.

The initial model supports synthetic 2D parallel-beam projections. Fan-beam, cone-beam, scanner-specific geometry, and real-data ingestion are deferred.

## 2. Image geometry versus acquisition geometry

Image-grid geometry and scanner acquisition geometry represent different concepts and must remain separate.

### 2.1 Image-grid geometry

The existing `ir::ImageGeometry2D` defines:

- Image width and height.
- Pixel spacing.
- Physical position of the center of pixel `(0, 0)`.
- Direction matrix describing the image axes in physical space.

The authoritative conventions are defined in [coordinate-conventions.md](coordinate-conventions.md).

For continuous image indices `(i, j)`, origin `o`, spacing `(sx, sy)`, and direction matrix `D`:

```text
r(i, j) = o + D [i * sx, j * sy]^T
```

The image origin refers to the center of pixel `(0, 0)`, not a corner.

### 2.2 Acquisition geometry

The initial 2D parallel-beam acquisition geometry describes:

- Projection angles and their acquisition order.
- Number of views.
- Number of detector samples per view.
- Detector sample spacing.
- Detector coordinate origin or center offset.
- Physical coordinate frame and units.
- Mapping between view/detector indices and geometric rays.

Acquisition geometry must not be added to `ir::ImageGeometry2D`. It must not own the sinogram sample buffer or perform file I/O.

## 3. Coordinate frame and angle conventions

Use a right-handed 2D physical coordinate frame with coordinates `(x, y)`.

- **Positive x:** Increasing physical x coordinate.
- **Positive y:** Increasing physical y coordinate.
- **Angle units:** Radians are the canonical internal unit.
- **Positive angle direction:** Counterclockwise from the positive x-axis.
- **Zero angle:** The projection normal points along positive x.
- **Detector coordinate:** Measured along the projection normal, with positive values along that normal.

The physical coordinate frame follows the image geometry supplied to the forward projector. Image index direction must not be assumed to equal physical coordinate direction when the image direction matrix is rotated or reflected.

### 3.1 Projection normal and ray direction

For projection angle `θ`, define the unit normal:

```text
n(θ) = (cos θ, sin θ)
```

Define a perpendicular unit ray direction:

```text
u(θ) = (-sin θ, cos θ)
```

The ray direction describes the orientation of the parallel rays. Reversing the direction along the same infinite line does not change an ideal line integral.

- `n(0) = (1, 0)`.
- `u(0) = (0, 1)`.
- Rays are parallel to the physical y-axis.
- Detector coordinates distinguish rays by their physical x position relative to the chosen reference center.

## 4. Reference center and detector coordinates

Define an acquisition reference center `c = (cx, cy)` in physical coordinates.

The reference center is a geometric reference for defining detector coordinates. It is not automatically the image origin or the physical center of the image.

For a point `r = (x, y)`, its signed detector coordinate for angle `θ` is:

```text
s = n(θ) · (r - c)
```

A ray identified by `(θ, s)` is the set of points satisfying:

```text
n(θ) · (r - c) = s
```

An equivalent parametric representation is:

```text
r(t) = c + s * n(θ) + t * u(θ)
```

where `t` ranges along the ray.

These equations define the initial idealized 2D parallel-beam geometry.

## 5. Detector sample indexing

Let:

- `D` be the number of detector samples per view.
- `d` be the detector index, with `0 <= d < D`.
- `Δs` be the detector spacing.
- `s0` be the detector coordinate of sample index zero.

The coordinate of detector sample `d` is:

```text
s(d) = s0 + d * Δs
```

Detector spacing must be finite and strictly positive. Its physical units must match the coordinate frame used by the projected image and the forward projector.

### 5.1 Centered detector convention

For a centered, uniformly sampled detector with zero detector-center offset, use:

```text
s(d) = (d - (D - 1) / 2) * Δs
```

This convention places the geometric center of the detector at coordinate zero.

For odd `D`, one sample lies at zero. For even `D`, zero lies halfway between the two central samples.

For example, with `D = 4` and `Δs = 2 mm`, the sample coordinates are:

```text
[-3 mm, -1 mm, 1 mm, 3 mm]
```

Implementations must not assume that detector index `D/2` always represents the detector center.

A non-zero detector offset may be represented by an explicitly defined center coordinate or by `s0`. The representation must be documented and must not apply the offset twice.

## 6. View angles and sinogram ordering

Let `V` be the number of projection views. Each view has an angle `θ[v]`, where `0 <= v < V`.

The initial data-model convention uses logical sinogram shape:

`[view, detector] = [V, D]`

The detector index changes fastest in contiguous row-major storage:

```text
linearIndex(v, d) = v * D + d
```

The sinogram sample at `(v, d)` represents the measurement associated with angle `θ[v]` and detector coordinate `s(d)`.

View ordering is preserved exactly as supplied by the acquisition geometry. Projection rows and their associated angle metadata must be reordered together if a reorder operation is explicitly performed.

The geometry contract must not silently sort angles, remove duplicate views, reverse detector order, or transpose the sinogram.

The initial model permits explicit angle arrays. A uniformly sampled angle sequence may be represented by generated angles only if the starting angle, increment, view count, and endpoint policy are fully specified.

## 7. Radon transform and transmission model

### 7.1 Parallel-beam Radon transform

Let `μ(r)` denote the linear attenuation coefficient at physical position `r`, expressed in inverse-length units. For the reference center `c`, projection angle `θ`, detector coordinate `s`, unit normal `n(θ)`, and perpendicular ray direction `u(θ)` defined above, the 2D parallel-beam Radon transform is:

```text
Rμ(θ, s) = ∫ μ(c + s * n(θ) + t * u(θ)) dt
```

The integral is over the entire ray, with `μ` taken as zero outside the modeled object support. For a finite image, the discrete implementation must define how rays intersect the image support and how samples outside that support are handled.

Each sinogram coordinate `(θ, s)` identifies one such line integral. The angle is the angle of the line's normal, not the ray direction. With this convention, equivalent parameterizations satisfy `Rμ(θ + π, s) = Rμ(θ, -s)`.

The Radon-transform equation defines the continuous geometric measurement; it does not prescribe a discrete ray/pixel intersection algorithm, interpolation method, or numerical quadrature rule. Those belong to the forward-projector contract.

When `μ` has units of inverse length and `dt` uses the corresponding length unit, `Rμ` is dimensionless. If a synthetic image instead stores normalized or arbitrary values, the value scale and resulting interpretation must be documented; it must not be presented as calibrated physical attenuation.

### 7.2 Beer–Lambert transmission

For an ideal monochromatic beam with incident intensity `I₀(θ, s)` and transmitted primary intensity `I(θ, s)`, the Beer–Lambert law gives:

```text
I(θ, s) = I₀(θ, s) * exp(-Rμ(θ, s))
```

Equivalently, when `I` and `I₀` are positive and refer to compatible, appropriately corrected measurements:

```text
Rμ(θ, s) = -ln(I(θ, s) / I₀(θ, s))
```

Thus the ideal sinogram used by the initial synthetic reconstruction workflow contains Radon line integrals, not transmitted intensities or detector counts. A synthetic forward projector may compute `Rμ` directly; it does not need to generate `I` and then take a logarithm. If transmission data are used in a future workflow, conversion to line integrals must be explicit and must validate the signal semantics and positive logarithm argument.

This model does not include photon-counting noise, scatter, detector response, or polychromatic beam hardening. For polychromatic X-rays, transmission is generally an energy-weighted integral of exponentials and its negative logarithm is not exactly the Radon transform of a single attenuation field. Real measurements therefore require a documented calibration/correction model before they can be treated as Radon data.

## 8. Known geometric examples for future tests

The following examples define geometric expectations, not the output of a particular discrete projector.

### Example A: Angle zero

Given `c = (0, 0)`, `θ = 0`, and detector coordinate `s = 2`:

- Normal: `n = (1, 0)`.
- Ray direction: `u = (0, 1)`.
- Ray equation: `x = 2`.
- Parametric ray: `r(t) = (2, t)`.

The ray is a vertical line at physical x coordinate 2.

### Example B: Quarter-turn

Given `c = (0, 0)`, `θ = π/2`, and detector coordinate `s = 3`:

- Normal: `n = (0, 1)`.
- Ray direction: `u = (-1, 0)`.
- Ray equation: `y = 3`.
- Parametric ray: `r(t) = (-t, 3)`.

The ray is a horizontal line at physical y coordinate 3.

### Example C: Centered detector with an even number of samples

Given `D = 4`, `Δs = 2`, and zero center offset:

- Detector index 0: `s = -3`.
- Detector index 1: `s = -1`.
- Detector index 2: `s = 1`.
- Detector index 3: `s = 3`.

No sample is located at the geometric detector center.

### Example D: Sinogram indexing

Given `V = 3` views and `D = 4` detector samples:

- Logical shape: `[3, 4]`.
- Sample `(v = 2, d = 1)` has linear index `2 * 4 + 1 = 9`.
- Total sample count: `3 * 4 = 12`.

These examples can be translated directly into focused geometry and indexing tests.

## 9. Compatibility with image geometry

The forward projector receives an image and its spatial geometry separately from the acquisition geometry.

For an image index `(i, j)`, first compute the physical location using `ir::ImageGeometry2D`. Projection rays are then defined in the physical coordinate frame using the acquisition geometry.

The forward projector must not assume that:

- The image origin equals the acquisition reference center.
- The image direction matrix is the identity.
- Image spacing equals detector spacing.
- Image rows always increase in the same direction as physical y.
- The image grid is centered at physical `(0, 0)`.

Any requirement that image and acquisition geometry use compatible physical units and coordinate frames must be validated or clearly stated as a precondition.

This document does not change existing registration coordinate conventions. Any change to those conventions requires a separate architecture decision record.

## 10. Validation expectations

A future acquisition-geometry implementation should validate, at minimum:

- The number of views is valid for the chosen data representation.
- The angle array length matches the declared view count.
- All required angle values are finite.
- Detector count is positive.
- Detector spacing is finite and strictly positive.
- Detector coordinate origin or center offset is finite.
- The detector-center convention is explicit.
- The coordinate units and frame are documented.
- The projection data dimensions match the geometry dimensions.
- View order and detector indexing are preserved consistently.

Additional algorithm-specific requirements, such as minimum angular coverage or restrictions on duplicate angles, must be stated by the consuming algorithm rather than silently imposed as universal geometry rules.

Invalid geometry must not be repaired by silently resizing data, guessing units, or changing coordinate conventions.

## 11. Future geometry extensions

### Fan-beam geometry

Fan-beam acquisition requires explicit source position, detector geometry, and source-to-object and source-to-detector relationships. It is outside the initial implementation scope.

### Cone-beam geometry

Cone-beam acquisition requires 3D source and detector geometry, detector-plane coordinate conventions, and volume-coordinate mapping. It is outside the initial implementation scope.

Future extensions should preserve explicit coordinate frames, units, detector indexing, and measurement semantics. They should not introduce a generic geometry hierarchy until a concrete second geometry model justifies it.

## 12. Design decisions and exclusions

The initial geometry contract adopts these decisions:

1. Image-grid geometry and acquisition geometry are distinct.
2. Physical image coordinates follow the existing `ImageGeometry2D` conventions.
3. Projection angles use radians and increase counterclockwise from the positive x-axis.
4. Angle zero has a normal along positive x and rays parallel to the y-axis.
5. Detector coordinates are signed distances along the projection normal from a defined reference center.
6. Centered detector coordinates use `(d - (D - 1) / 2) * Δs`.
7. Sinogram layout is `[view, detector]`, with detector samples contiguous.
8. View order and angle metadata remain aligned.
9. The continuous Radon-transform equation and its Beer–Lambert interpretation are specified independently of the discrete projector implementation.
10. Fan-beam, cone-beam, scanner-specific metadata, and real-data ingestion are deferred.

## 13. Acceptance criteria

DOC-04 is complete when:

- [ ] Image lattice geometry and acquisition geometry are distinct.
- [ ] Physical coordinate frame and angle conventions are explicit.
- [ ] Detector coordinate equations and center conventions are explicit.
- [ ] Sinogram view order, detector order, and indexing are defined.
- [ ] Forward projection equations agree with the stated conventions.
- [ ] Radon line integrals are distinguished from Beer–Lambert transmitted intensities and raw detector measurements.
- [ ] Known geometric examples are provided for future unit tests.
- [ ] Units and geometry compatibility requirements are documented.
- [ ] Existing registration coordinate conventions remain unchanged.
- [ ] Future fan-beam and cone-beam geometry are identified as deferred.
- [ ] The document agrees with DOC-02, DOC-03, and [coordinate-conventions.md](coordinate-conventions.md).

## 14. Definition of done

Review this document against the existing coordinate conventions and the reconstruction data model. Resolve inconsistencies before implementing acquisition-geometry types or projection algorithms.

The resulting contract must allow future implementation issues to define focused Catch2 tests for detector coordinates, angle conventions, ray equations, geometry compatibility, and sinogram indexing without relying on visual plausibility alone.