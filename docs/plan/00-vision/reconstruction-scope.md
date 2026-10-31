# CT Image Reconstruction — Scope and Requirements

- **Status:** Initial scope baseline
- **Backlog ID:** DOC-01
- **Domain:** CT image reconstruction
- **Applies to:** First reconstruction milestones (M0–M1 planning baseline)

## 1. Purpose

Introduce a CT image-reconstruction domain into ImageRegistration. The first deliverable is a small, numerically testable 2D parallel-beam reconstruction pipeline that can be developed independently of the existing image-registration pipeline.

The initial end-to-end demonstration is:

"Shepp–Logan phantom → simulated parallel-beam projections → filtered backprojection (FBP) → numerical validation"

The purpose of this phase is to establish correct data contracts, geometry, algorithms, and numerical behavior. It is not a clinical reconstruction product and must not imply diagnostic suitability.

## 2. Scope baseline

| Dimension | Initial decision |
| --- | --- |
| Modality | X-ray CT |
| Reconstruction dimensionality | 2D |
| Acquisition geometry | 2D parallel beam |
| Initial input | Synthetic 2D phantom |
| Projection generation | Software forward projector |
| Reconstruction algorithm | Filtered backprojection (FBP) |
| Validation | Deterministic numerical tests and phantom-based metrics |
| Integration | Reconstruction is a domain within this repository, not a nested Git submodule |
| Real-world input | Deferred to a later phase |

The first milestone uses a known synthetic object so that projection generation and reconstruction can be validated without scanner-specific metadata, file-format handling, or clinical datasets.

## 3. Initial user workflows

### UC-01 — Reconstruct a synthetic phantom

A developer creates a deterministic 2D Shepp–Logan phantom, defines parallel-beam acquisition geometry, generates projections, runs FBP, and receives a reconstructed 2D image.

Success criteria:

- All stages execute with explicitly defined dimensions, units, and geometry conventions.
- The output is finite and has the expected image geometry.
- The result can be evaluated against the original phantom using documented numerical metrics.

### UC-02 — Inspect intermediate projection data

A developer can inspect generated projection data as a sinogram, including detector and view dimensions and the associated geometry.

Success criteria:

- Detector and view axes are unambiguous.
- Index-to-coordinate conventions are documented.
- Projection data can be tested independently of the reconstruction algorithm.

### UC-03 — Validate numerical behavior

A developer runs deterministic tests against known synthetic cases and measures reconstruction error against the original phantom.

Success criteria:

- Tests verify geometry, operator behavior, and numerical stability.
- A baseline reconstruction metric is documented.
- Tolerances account for discretization; approximate numerical reconstruction is not expected to equal the original image exactly.

## 4. Functional requirements

Priorities: P0 is required for the first synthetic pipeline; P1 is a near-term quality requirement; P2 is deferred.

| ID | Priority | Requirement | Verification |
| --- | --- | --- | --- |
| FR-01 | P0 | Represent a 2D scalar image with explicit width, height, pixel spacing, origin, and orientation/direction conventions. | Unit tests validate dimensions and geometry; invalid geometry is rejected or reported. |
| FR-02 | P0 | Represent 2D parallel-beam acquisition geometry with explicit projection angles and detector sampling. | Tests verify angle count/order, detector count, spacing, and coordinate conventions. |
| FR-03 | P0 | Generate a deterministic Shepp–Logan phantom for tests and examples. | Tests verify dimensions, finite pixel values, and known intensity properties. |
| FR-04 | P0 | Compute simulated parallel-beam line-integral projections from a 2D image. | Tests cover zero images, simple shapes, symmetry, and deterministic output. |
| FR-05 | P0 | Store projection samples in a sinogram with explicit detector and view axes and associated acquisition geometry. | Tests verify index-to-axis mapping and metadata consistency. |
| FR-06 | P0 | Apply a documented ramp filter along the detector axis for FBP. | Tests verify output size, finite values, and a documented filter property. |
| FR-07 | P0 | Backproject filtered projections using the same declared parallel-beam geometry. | Tests verify geometry consistency and finite output. |
| FR-08 | P0 | Compose filtering and backprojection into an FBP operation. | An end-to-end phantom test computes a reconstruction error metric. |
| FR-09 | P0 | Define reconstruction output as a 2D image with explicit geometry and documented value interpretation. | Tests verify output dimensions, geometry, and finite values. |
| FR-10 | P1 | Expose intermediate projection, filtering, and backprojection stages sufficiently for isolated testing and diagnosis. | Each stage is tested without invoking the complete pipeline. |
| FR-11 | P1 | Report invalid arguments and unsupported geometry using documented error-handling conventions. | Negative tests cover invalid dimensions, spacing, angles, and incompatible data. |
| FR-12 | P1 | Provide a reproducible example that runs the synthetic pipeline and reports numerical metrics. | Example builds and runs using documented CMake options. |

## 5. Non-functional requirements

| ID | Priority | Requirement | Verification |
| --- | --- | --- | --- |
| NFR-01 | P0 | Keep reconstruction components modular and composable. Avoid a monolithic class that owns every stage. | Architecture review confirms distinct responsibilities. |
| NFR-02 | P0 | Keep numerical behavior deterministic for fixed inputs and configuration. | Repeat-run tests produce equivalent results within documented tolerances. |
| NFR-03 | P0 | Use explicit coordinate, axis, angle, spacing, and unit conventions. | Documentation and tests exercise the conventions. |
| NFR-04 | P0 | Keep the core reconstruction domain independent of DICOM, visualization, GUI, and scanner-vendor I/O. | Dependency review and core build verify the boundary. |
| NFR-05 | P1 | Make algorithms independently testable and support alternative implementations through focused interfaces or composition where justified. | Tests instantiate stages independently. |
| NFR-06 | P1 | Document algorithmic approximations, discretization choices, and numerical limitations. | Algorithm documentation specifies sampling, interpolation, filtering, and normalization. |
| NFR-07 | P1 | Follow the project's C++20, Eigen, Catch2, and coding standards. | Build, tests, and CI checks pass. |
| NFR-08 | P1 | Avoid premature GPU, threading, plugin, and generic N-dimensional abstractions. | Architecture review confirms no speculative infrastructure. |

## 6. Explicit non-goals for the initial release

The following are not requirements for the first synthetic 2D parallel-beam pipeline:

- Fan-beam and cone-beam acquisition geometries.
- 3D volume reconstruction, helical CT, or multi-slice reconstruction.
- MRI acquisition or MRI reconstruction.
- Sparse-view reconstruction studies and limited-angle recovery.
- Iterative reconstruction algorithms such as ART, SART, and statistical/model-based methods.
- Noise and scatter simulation, beam-hardening correction, or other physical-system corrections.
- DICOM parsing, writing, or conversion.
- Raw scanner acquisition ingestion.
- Reading or validating the DICOM-CT-PD dataset as part of the initial pipeline.
- Scanner calibration, vendor-specific protocols, and clinical workflow integration.
- Regulatory, diagnostic, or clinical-performance claims.
- GPU acceleration, distributed processing, or performance optimization before a measured baseline exists.

These are potential later work items, not implied capabilities or commitments of the initial API.

## 7. Assumptions and constraints

1. **Synthetic-first validation:** A deterministic phantom is sufficient to validate initial algorithmic contracts. It does not establish clinical validity.
2. **Idealized physics:** Initial simulated projections represent line integrals through an idealized 2D attenuation image. They do not model a complete scanner or polychromatic X-ray physics.
3. **Parallel-beam geometry:** Each view is represented by a projection angle and a linearly sampled detector coordinate. Angle intervals, orientation, and detector-center conventions must be documented and tested.
4. **Units:** Image pixel spacing and detector spacing must be explicit. The implementation must not silently mix pixel units, physical length units, attenuation coefficients, and dimensionless phantom values.
5. **Numerical approximation:** Forward projection, filtering, interpolation, and backprojection are discrete approximations. Their sampling and normalization conventions must be specified.
6. **No raw-count assumption:** Synthetic line-integral projections are not detector photon counts and must not be labeled or serialized as such.
7. **Domain boundary:** CT reconstruction belongs in this repository as a peer domain to registration. It is not a nested Git submodule.
8. **No clinical claim:** Output is for algorithm development and numerical research only unless a separately governed validation and regulatory process establishes otherwise.

## 8. Measurement and data terminology

These terms represent different physical quantities and must not be used interchangeably.

| Term | Meaning | Typical representation |
| --- | --- | --- |
| Detector counts | Number of detected X-ray photons, or detector signal calibrated to counts, recorded for each detector element and view. These are acquisition measurements affected by exposure, detector response, and noise. | Raw or calibrated detector measurement array indexed by view and detector channel. |
| Incident intensity, I₀ | Reference signal expected without the object under the relevant acquisition conditions. | Calibration/reference measurement, potentially detector- and view-dependent. |
| Transmitted intensity, I | Detector signal measured after the X-ray beam passes through the object. | Measured transmission signal. |
| Corrected transmission | Transmission after applicable acquisition corrections, such as offset and gain/flat-field correction. The exact correction chain is system-dependent. | Corrected detector-domain signal; not yet a line integral. |
| Line integral / projection value | In the ideal monoenergetic model, the negative logarithm of normalized transmission: `p = -ln(I/I₀)`. It approximates the path integral of the linear attenuation coefficient along the ray. | Real-valued projection sample indexed by view and detector position. |
| Sinogram | Arrangement of projection samples by detector position and projection angle. It is a data organization of projection values, not necessarily raw detector counts. | 2D array plus explicit acquisition geometry and axis conventions. |
| Reconstructed image | Estimate of the spatial attenuation distribution derived from projection data. | 2D image with spatial geometry; values represent estimated attenuation or a documented scaled/relative quantity. |
| DICOM CT image | Image object encoded using the DICOM information model and pixel-data representation, with metadata describing image and acquisition context. It is not synonymous with raw detector measurements or a sinogram. | DICOM instance(s), pixel data, and relevant metadata. |

> **Important distinction:** The initial synthetic pipeline begins with a phantom and produces idealized line-integral projection values directly. It does not begin with detector counts, perform detector corrections, or read a DICOM CT image.

A reconstructed DICOM CT image is an output representation of reconstructed spatial image data. It is fundamentally different from raw projection/acquisition data. A DICOM CT series must not be treated as if it were a detector sinogram.

## 9. Candidate acceptance-test scenarios

These scenarios define observable behaviors for later implementation issues. This documentation issue does not require production code.

| ID | Scenario | Expected result |
| --- | --- | --- |
| ATS-01 | Generate a Shepp–Logan phantom with valid dimensions. | Correct dimensions; finite, deterministic pixel values. |
| ATS-02 | Generate the same phantom twice with the same parameters. | Equivalent pixel values. |
| ATS-03 | Configure parallel-beam geometry with valid angles and detector sampling. | Requested view and detector counts are preserved, with documented conventions. |
| ATS-04 | Configure geometry with zero detector spacing or invalid dimensions. | A documented error is returned; invalid geometry is not silently accepted. |
| ATS-05 | Forward-project an all-zero image. | All line-integral projection samples are zero within numerical tolerance. |
| ATS-06 | Forward-project a non-zero phantom. | Expected sinogram dimensions; finite and deterministic values. |
| ATS-07 | Forward-project a symmetric phantom using symmetric views. | Projection symmetry follows the documented angle and detector conventions within discretization tolerance. |
| ATS-08 | Filter a valid sinogram. | Detector-axis length and geometry remain consistent; output is finite and follows the specified ramp-filter convention. |
| ATS-09 | Backproject compatible filtered projections. | Output image has requested geometry and finite pixel values. |
| ATS-10 | Run FBP with projections and geometry from the same configuration. | A finite reconstructed image is produced with the requested geometry. |
| ATS-11 | Run the complete phantom → projections → FBP pipeline. | A documented image-error metric is computed against the source phantom. |
| ATS-12 | Supply a sinogram whose geometry is incompatible with the reconstruction request. | The operation fails explicitly rather than silently reinterpreting the data. |
| ATS-13 | Repeat the complete pipeline with identical inputs and parameters. | Equivalent reconstruction and metric within documented numerical tolerances. |
| ATS-14 | Supply invalid or non-finite input samples where the API disallows them. | Behavior follows the documented validation/error contract. |

The project must establish a baseline metric and tolerance before treating ATS-11 as a pass/fail numerical gate. The threshold must be justified by discretization, sampling, and normalization choices—not selected merely to make a test pass.

## 10. Architecture and evolution constraints

The reconstruction domain follows the existing project principles:

- Data before algorithms: Define image, acquisition geometry, and projection data contracts before implementing the reconstruction pipeline.
- Separation of concerns: Keep geometry, projection data, forward projection, filtering, backprojection, and orchestration as separate responsibilities.
- Composition over inheritance: Compose focused stages; avoid deep class hierarchies.
- Dependency inversion: High-level reconstruction orchestration should depend on stable, narrow contracts rather than concrete I/O or visualization systems.
- Minimal core dependencies: Keep DICOM, VTK, ITK, OpenCV, Qt, and vendor SDKs out of the reconstruction core.
- Incremental evolution: Add later acquisition geometries and dimensions as separately designed capabilities. Do not force a generic N-dimensional abstraction prematurely.
- Domain boundary: CT reconstruction is part of this repository, not a Git submodule. Reuse registration types only when their semantics genuinely match; do not couple distinct domain concepts merely to reduce code duplication.

The scope baseline is consistent with the existing design principles and the project's preference for independent, composable domains.

## 11. Planned evolution (not initial requirements)

| Phase | Capability | Scope note |
| --- | --- | --- |
| M0 | Synthetic 2D parallel-beam foundation | Phantom, projection data model, geometry, and baseline pipeline contracts. |
| M1 | Deterministic FBP pipeline | Forward projector, ramp filter, backprojection, FBP, numerical metrics, and validation. |
| M2 | Sparse-view and iterative methods | Separate experiments and algorithms; do not alter the initial FBP contract implicitly. |
| M3 | Real-data investigation and DICOM-CT-PD | Dataset study, input semantics, reader/adapter, and validation against real data. |
| M4 | Benchmarks and optimization | Measure reference behavior first; optimize only with evidence and preserved numerical correctness. |
| Later | Fan-beam, cone-beam, 3D, MRI | Independent scope/design decisions and backlog phases. |

## 12. Definition of scope completion

This scope document is complete when:

- The initial modality, dimensionality, acquisition geometry, and end-to-end workflow are explicit.
- Initial use cases, functional requirements, non-functional requirements, assumptions, and non-goals are stated.
- Requirements have priorities and observable verification methods.
- Detector counts, corrected transmission, line integrals, sinograms, reconstructed images, and DICOM CT images are distinguished.
- Candidate acceptance-test scenarios are documented without claiming production implementation.
- The domain boundary and evolution path agree with the repository's design principles.