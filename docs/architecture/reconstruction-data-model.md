# CT Reconstruction Data Model and Measurement Semantics

- Backlog ID: DOC-03
- Related issue: #35
- Depends on: #33 (DOC-01), #34 (DOC-02)
- Status: Data-model proposal
- Initial scope: 2D parallel-beam CT reconstruction

## 1. Purpose

This document defines the data and measurement contracts for the initial CT reconstruction workflow in ImageRegistration.

The data model must make the following workflow unambiguous:

> 2D attenuation image → simulated line-integral projections → sinogram → filtered backprojection → reconstructed image

It also establishes boundaries for future real-data input without requiring scanner-specific I/O or calibration implementations in the initial milestone.

The primary goal is to prevent invalid operations caused by confusing arrays that have the same shape but represent different physical quantities.

This document specifies intended contracts. It does not claim that the proposed reconstruction-specific types have already been implemented.

## 2. Existing image types and conventions

The reconstruction domain must reuse the existing types where their semantics match:

- `ir::Image2D` — an owning 2D scalar image backed by a contiguous `std::vector<double>`.
- `ir::ImageGeometry2D` — image dimensions, spacing, origin, and direction.
- [Coordinate conventions](coordinate-conventions.md) — authoritative image coordinate and indexing rules.

Do not create a second reconstruction-specific image or spatial-geometry type merely to represent CT images.

### 2.1 Image dimensions and storage

For an image with width `W` and height `H`:

- Valid indices are `i ∈ [0, W)` and `j ∈ [0, H)`.
- Storage is row-major.
- The linear index is `index = j * W + i`.
- The `i` index changes fastest in memory.
- Pixel values are `double`.

The image geometry defines the physical position of pixel centers, spacing, origin, and direction. The units of spacing remain application-defined and must be used consistently.

### 2.2 Image value semantics

An image's pixel type does not, by itself, define the physical meaning of its values.

For the initial synthetic CT workflow, the phantom represents a scalar attenuation-like field under a documented scale convention. A forward projector integrates that field along rays.

The resulting line-integral projections are not detector counts and must not be described as such.

A reconstructed image must document whether its values represent an attenuation estimate, a relative quantity, or another explicitly defined scale.

## 3. Projection data terminology

The following measurement types have distinct meanings and must not be silently substituted for one another.

| Representation | Meaning | Initial milestone |
| --- | --- | --- |
| Raw detector measurements | Detector signals or counts recorded for each detector element and view | Deferred |
| Incident reference, `I₀` | Reference signal under the relevant acquisition conditions | Deferred |
| Transmitted measurement, `I` | Signal recorded after transmission through the object | Deferred |
| Corrected transmission | Transmission after an explicitly defined correction chain | Deferred |
| Logarithmic line integral | `p = -ln(I/I₀)` under the applicable transmission model | Required for synthetic projection workflow |
| Filtered projections | Line-integral projection samples after the configured reconstruction filter | Required as an algorithm intermediate |
| Reconstructed image | Spatial estimate produced by a reconstruction algorithm | Required |

The formula `p = -ln(I/I₀)` applies only when the signals and corrections support that interpretation and the logarithm's argument is valid and positive. It is not a generic conversion for arbitrary detector values.

The initial synthetic pipeline generates idealized line-integral samples directly. It does not simulate raw photon counts, perform detector corrections, or read scanner acquisition files.

## 4. Projection array layout

The initial projection representation is a two-dimensional sinogram for 2D parallel-beam acquisition.

Define:

- `D` — number of detector samples per view.
- `V` — number of projection views.
- `d` — detector index, where `0 ≤ d < D`.
- `v` — view index, where `0 ≤ v < V`.

### 4.1 Axis ordering

Use the logical shape:

`[view, detector] = [V, D]`

The detector axis is the last logical axis and is contiguous in memory.

For a row-major contiguous buffer:

`linearIndex(v, d) = v * D + d`

Consequences:

- Incrementing `d` by one accesses the next detector sample within the same view.
- Incrementing `v` advances to the next view.
- The total sample count is `V * D`.
- A sinogram with `V` views and `D` detector samples contains `V * D` samples.

This convention is deliberately different from image indexing: an image uses `(i, j)` with width-index `i` changing fastest; a sinogram uses `(v, d)` with detector-index `d` changing fastest.

Any future API that exports a transposed or differently strided representation must state that explicitly.

### 4.2 Illustrative layout

For three views and four detector samples:

Logical rows are views; columns are detector indices.

```text
             detector index
             d=0   d=1   d=2   d=3
view v=0     p00   p01   p02   p03
view v=1     p10   p11   p12   p13
view v=2     p20   p21   p22   p23

Linear buffer:
[p00, p01, p02, p03, p10, p11, p12, p13, p20, p21, p22, p23]
```

The values shown are illustrative samples, not prescribed numerical results.

## 5. Acquisition geometry

Projection samples cannot be interpreted correctly without their acquisition geometry.

For the initial 2D parallel-beam model, geometry must specify:

- Number of views.
- Projection angle for each view.
- Angle units and angular convention.
- Detector sample count.
- Detector spacing.
- Detector coordinate origin or center convention.
- The coordinate frame in which angles and detector positions are defined.
- Consistency with the spatial coordinate convention of the projected image.

### 5.1 Angle conventions

Use radians as the canonical internal angle unit for reconstruction algorithms.

The geometry must document:

- The positive direction of rotation.
- The reference direction for angle zero.
- Whether angles are stored in acquisition order or sorted order.
- Whether repeated or equivalent angles are permitted.
- The relationship between an angle and its corresponding sinogram row.

Do not silently sort projection rows or reorder geometry metadata independently. If projections are reordered, the associated angles and all per-view metadata must be reordered identically.

### 5.2 Detector coordinates and spacing

Detector spacing represents the physical distance between adjacent detector sample locations.

Its units must be compatible with the coordinate system used by the forward projector and reconstruction geometry.

The model must distinguish:

- Detector sample index.
- Detector sample coordinate.
- Detector spacing.
- Detector center or coordinate origin.

Do not assume that the detector's central sample is always located at index `D / 2`. For even detector counts, a geometric center may lie between two samples. The chosen convention must be explicit.

### 5.3 Geometry compatibility

A reconstruction operation must not combine projection data with incompatible acquisition geometry.

At minimum, validation must verify:

- Projection view count equals the geometry's view count.
- Projection detector count equals the geometry's detector count.
- The angle list has the expected number of entries.
- Required sampling parameters are valid.
- Required metadata is present.
- The measurement representation is supported by the requested algorithm.

Compatibility checks must not silently resize arrays, reorder views, or reinterpret units.

## 6. Measurement semantics and units

A numerical array is not a complete measurement contract. The representation must identify what each sample means.

### 6.1 Line-integral projections

For the initial synthetic workflow, each sinogram sample represents an idealized line integral:

`p(θ, s) = ∫ μ(r) dl`

where:

- `θ` is the projection angle.
- `s` is the detector coordinate.
- `μ(r)` is the scalar attenuation-like field.
- `dl` is an infinitesimal distance along the ray.

If `μ` is expressed in inverse-length units and `dl` uses the corresponding length unit, the line integral is dimensionless.

If the phantom uses normalized or arbitrary relative values instead, the model must document that convention. Do not label normalized values as calibrated physical attenuation measurements.

### 6.2 Raw measurements and transmission data

Future real-data adapters may encounter raw counts, detector signals, reference intensities, corrected transmission, or already-computed line integrals.

These representations must not be stored in an unqualified projection array whose meaning is inferred from context.

The data model must provide an explicit semantic distinction, for example through separate domain types or a strongly typed measurement-kind field with well-defined invariants.

A measurement-kind field alone is not sufficient if the required metadata for interpreting that kind is missing.

### 6.3 Filtered projection data

Filtering transforms projection values according to a defined numerical operation.

Filtered samples must not be confused with unfiltered input measurements. The filtering operation must document:

- The input measurement representation it accepts.
- The detector axis along which it operates.
- The filter definition and normalization convention.
- Whether output storage owns a new buffer.
- Which geometry and metadata remain applicable to the filtered samples.

The initial ramp filter is intended for the FBP workflow and must operate along the detector axis.

## 7. Proposed reconstruction data contracts

The following are logical contracts, not mandatory class names or an instruction to create a separate class for every concept.

### 7.1 `ir::Image2D`

Reuse the existing type for:

- Synthetic phantom samples.
- Spatial image inputs to the forward projector.
- Reconstructed image outputs.

Use `ir::ImageGeometry2D` for spatial dimensions and coordinate mapping.

Do not add acquisition angles, detector counts, scanner calibration, or file-format fields to `Image2D`.

### 7.2 Parallel-beam acquisition geometry

A future reconstruction-specific geometry type should contain the information required to interpret a 2D parallel-beam sinogram.

Its contract must include view angles, detector sampling, coordinate conventions, and dimensional consistency.

The geometry type must not own projection sample values or perform file I/O.

### 7.3 Sinogram / projection data

A sinogram contract must specify:

- Sample dimensions and indexing.
- Contiguous memory layout.
- Sample value type.
- Measurement semantics.
- Applicable units or scaling convention.
- Acquisition geometry or an explicit, reliable association with it.
- Validation rules for dimensions and metadata.
- Ownership and mutation rules.

The initial synthetic pipeline requires a representation for line-integral projection values.

Future raw detector measurements must not be accepted by a line-integral-only algorithm without an explicit, validated conversion step.

### 7.4 Filtered projection output

Filtered projection data must preserve a documented relationship to its input sinogram and acquisition geometry.

The filtering operation must return a result with known measurement semantics. If the filtered samples no longer represent unfiltered line integrals, the result must not be mislabeled as the original input representation.

### 7.5 Reconstructed image output

The output is an `ir::Image2D` with explicit spatial geometry.

The reconstruction operation must document:

- Output width and height.
- Pixel spacing, origin, and direction.
- Interpretation and scaling of output values.
- The acquisition geometry and reconstruction configuration used.
- Numerical limitations and approximations.

The output image is not itself a sinogram or raw detector measurement.

## 8. Metadata and provenance

Metadata must preserve the information required to interpret and validate the projection samples.

For the initial synthetic workflow, record or make explicitly available:

- Acquisition geometry.
- Projection angle values and ordering.
- Detector sampling and coordinate conventions.
- Measurement semantics and units or scaling.
- Image geometry used by the forward projector.
- Relevant algorithm configuration needed to reproduce the generated projections.

For future real-data workflows, adapters may also need to preserve:

- Source identity or provenance.
- Calibration/reference data or references to it.
- Applied correction steps.
- Source measurement units and scaling.
- Acquisition parameters needed to interpret the data.
- Missing, estimated, or unavailable metadata.

Metadata must not claim a calibration or correction was performed when it was not.

The initial synthetic workflow does not require a general provenance database, scanner metadata framework, or DICOM abstraction.

## 9. Ownership, mutability, and memory behavior

The initial implementation should prefer simple ownership and predictable lifetimes.

### 9.1 Owning data

Owning image and projection objects must retain their sample buffers for their own lifetime.

For the initial 2D implementation, contiguous storage backed by `std::vector<double>` is consistent with the existing `Image2D` design and is a reasonable default for sinogram storage.

Copying an owning data object must not create two objects that unexpectedly share mutable storage.

### 9.2 Mutability

Geometry and sample mutation have different consequences.

- Image geometry is fixed at construction in the current `ImageGeometry2D` design.
- `Image2D` exposes writable pixel access.
- Projection data should document whether samples are writable after construction.
- Geometry or metadata changes must not silently invalidate the relationship between projection samples and acquisition geometry.

Do not introduce immutable wrappers, shared ownership, or copy-on-write infrastructure without a demonstrated need.

### 9.3 Non-owning views

A future API may expose non-owning views for performance or interoperability, but such views must document:

- Which object owns the underlying memory.
- That the view does not extend the owner's lifetime.
- Whether the view is writable.
- Stride and axis-order assumptions.
- Invalidation rules when the owner is moved, resized, destroyed, or otherwise modified.

The initial model does not require a general-purpose view framework.

### 9.4 Large-array behavior

For a contiguous sinogram, the sample buffer contains `V * D` elements.

With `double` samples, the raw sample-buffer storage is approximately:

`8 * V * D bytes`

This estimate excludes object overhead, geometry, metadata, temporary buffers, and allocator overhead.

Implementations must check dimensions and allocation-size calculations for overflow before allocating storage. They must not assume that every valid mathematical shape is practical to allocate.

The initial implementation should avoid unnecessary copies in algorithm pipelines where ownership can be transferred safely, but performance abstractions must not compromise clear lifetime rules.

## 10. Validation and invalid metadata

The data model must define how invalid or incomplete inputs are handled.

Validation should cover:

- Zero or negative dimensions.
- Invalid detector spacing.
- Invalid or non-finite projection angles.
- Incorrect sample-buffer size.
- Missing required geometry.
- Inconsistent projection dimensions and geometry.
- Unsupported measurement semantics.
- Non-finite sample values where the consuming operation prohibits them.
- Invalid reference or transmission values for any future logarithmic conversion.
- Overflow in sample-count or allocation-size calculations.

Validation must be explicit and must follow the repository's established error-handling conventions.

Do not introduce a new global exception hierarchy solely for this feature. The appropriate return or error mechanism should be reconciled with the existing code before implementation.

A model must not silently replace missing metadata with plausible-looking defaults when doing so changes the meaning of the measurements.

## 11. Synthetic and future real-data paths

### 11.1 Initial synthetic path

```text
ir::Image2D phantom
        |
        | spatial geometry + parallel-beam geometry
        v
Forward projector
        |
        v
Line-integral sinogram
        |
        v
Projection filter
        |
        v
Filtered projections
        |
        v
Backprojector
        |
        v
ir::Image2D reconstructed image
```

The forward projector produces idealized line-integral samples directly. No detector-count conversion or calibration stage is implied.

### 11.2 Future real-data path

```text
External acquisition source
        |
        v
Format-specific I/O adapter
        |
        v
Measurement interpretation and validation
        |
        v
Explicitly represented projection data
        |
        v
Compatible reconstruction algorithm
```

The adapter must identify the source measurement semantics and preserve the metadata needed to interpret them.

If conversion from transmission measurements to logarithmic line integrals is required, it must be an explicit operation with documented preconditions and correction assumptions.

A reconstructed DICOM CT image is spatial image data, not a raw projection sinogram. It must not be passed into the reconstruction pipeline as if it were detector measurements.

Real scanner ingestion, DICOM parsing, calibration, and DICOM-CT-PD support remain outside the initial synthetic milestone.

## 12. Future 3D extension

The initial model is explicitly 2D.

A future 3D model may introduce volume samples, three-dimensional acquisition geometry, and detector arrays with additional axes. That extension must preserve the principles established here:

- Explicit axis names and order.
- Explicit memory layout.
- Explicit spatial and acquisition coordinate conventions.
- Explicit measurement semantics.
- Explicit units or scaling.
- Explicit ownership and lifetime rules.

Do not introduce a generic N-dimensional tensor abstraction solely to anticipate 3D support.

A future 3D extension must be designed and tested against its actual acquisition geometry and algorithm requirements.

## 13. Testing implications

The data-model design must enable deterministic tests for:

| Test area | Expected behavior |
| --- | --- |
| Image reuse | Reconstruction uses existing `ir::Image2D` and `ir::ImageGeometry2D` |
| Sinogram dimensions | View and detector counts are preserved |
| Indexing | `(v, d)` maps to `v * D + d` |
| Storage | Detector samples are contiguous within each view |
| Measurement semantics | Raw measurements cannot be silently accepted as line integrals |
| Geometry consistency | Mismatched dimensions or metadata are rejected |
| View ordering | Projection rows remain aligned with their angles |
| Units and scaling | The documented convention is available and unambiguous |
| Invalid inputs | Invalid dimensions, spacing, angles, or prohibited sample values follow documented error behavior |
| Ownership | Owning buffers remain valid for the object's lifetime |
| Mutation | Data changes do not silently invalidate geometry or measurement metadata |
| Allocation safety | Sample-count and allocation-size calculations are checked |
| Image output | Reconstruction output has the requested spatial geometry |

These are test requirements for implementation issues. This documentation task does not require the production data types or reconstruction algorithms to be implemented.

## 14. Design decisions and exclusions

The initial data model adopts these decisions:

1. Reuse existing spatial image types.
2. Use a concrete 2D sinogram model with `[view, detector]` logical axis order.
3. Store detector samples contiguously within each view.
4. Distinguish measurement semantics explicitly.
5. Associate projection data with validated acquisition geometry.
6. Document units, scaling, and coordinate conventions.
7. Prefer owning contiguous buffers and clear lifetimes.
8. Permit future non-owning views only with explicit lifetime and stride contracts.
9. Keep file parsing and scanner-specific interpretation outside numerical algorithms.
10. Defer generic N-dimensional storage, raw-data ingestion, DICOM integration, and calibration frameworks.

The exact C++ type names, constructors, and error signatures should be reconciled with repository conventions during implementation. They should not be finalized merely to satisfy this documentation issue.

## 15. Acceptance criteria

DOC-03 is complete when:

- [ ] Image and volume sample semantics are documented, with current 2D scope explicit.
- [ ] Existing image geometry and storage conventions are reused.
- [ ] Projection array axis order, indexing, and memory layout are unambiguous.
- [ ] Detector counts, corrected transmission, line integrals, filtered projections, and reconstructed images are distinguished.
- [ ] Units and scaling conventions are documented.
- [ ] Acquisition metadata and view ordering requirements are specified.
- [ ] Ownership, mutability, non-owning views, and buffer lifetime rules are documented.
- [ ] Invalid or missing metadata and incompatible geometry have defined validation expectations.
- [ ] The model supports a separately designed 3D extension without requiring a generic N-dimensional framework now.
- [ ] The document agrees with DOC-01, DOC-02, and the existing coordinate conventions.
- [ ] Proposed contracts are clearly distinguished from implemented code.

## 16. Definition of done

The document is reviewed against the current repository's image types, coordinate conventions, architecture, and coding practices. Any conflict is resolved before concrete reconstruction types are introduced.

The resulting data contract is sufficiently precise for follow-up implementation issues to define focused Catch2 tests before production code, while keeping the existing registration library and its tests independent.