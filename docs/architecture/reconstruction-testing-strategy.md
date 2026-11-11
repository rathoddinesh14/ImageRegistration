# Reconstruction Testing Strategy and Numerical Validation

- **Backlog ID:** DOC-06
- **Related issue:** #38
- **Dependencies:** #35 (DOC-03), #37 (DOC-05)
- **Status:** Proposed testing strategy
- **Scope:** 2D parallel-beam CT reconstruction
- **Numerical precision:** IEEE 754 double precision (`double`)
- **Testing framework:** Catch2 v3.5.2

---

## 1. Purpose

This document defines the testing methodology and numerical validation strategy for the initial CT reconstruction module in ImageRegistration.

The primary objective is to establish reliable numerical correctness before implementing reconstruction algorithms.

The strategy covers:

- Unit and structural testing.
- Analytical validation of projection operations.
- Forward and adjoint operator consistency.
- End-to-end synthetic phantom reconstruction.
- Independent reference comparisons.
- Numerical error metrics and tolerance policies.
- Deterministic testing and reproducibility.
- Integration testing.
- Performance and memory benchmarking.

The testing strategy must distinguish mathematical correctness from numerical approximation and implementation-specific behavior.

A reconstruction result that appears visually plausible is not sufficient evidence of numerical correctness.

## 2. Testing Philosophy

The reconstruction module follows these principles:

1. **Mathematical correctness first:** Every numerical operation must have a clearly defined mathematical contract.
2. **Test before implementation:** Follow the repository's Red-Green-Refactor workflow.
3. **Deterministic results:** Tests must be reproducible using fixed inputs, random seeds, and reference fixtures.
4. **Independent validation:** Expected results must not rely exclusively on the implementation being tested.
5. **Explicit tolerances:** Every approximate numerical comparison must have a documented tolerance and rationale.
6. **Separation of concerns:** Unit tests, operator tests, reconstruction tests, and benchmarks must remain distinguishable.
7. **No visual-only acceptance:** Visual inspection is supplementary, never the sole correctness criterion.

### 2.1 Types of Numerical Error

Three fundamentally different error sources must be considered.

| Error type | Description | Example |
|---|---|---|
| Floating-point error | Error introduced by finite-precision arithmetic | Accumulation of ray contributions |
| Discretization error | Difference between continuous and discrete mathematical models | Pixel-based approximation of a continuous line integral |
| Algorithmic error | Error introduced by numerical approximations | Interpolation, filtering, boundary handling |

These error sources must not be treated as interchangeable.

In particular, increasing a floating-point tolerance must not be used to conceal a discretization or algorithmic defect.

## 3. Testing Levels

The reconstruction module adopts the following testing hierarchy.

| Level | Category | Primary objective |
|---|---|---|
| T0 | Unit and structural | Validate data structures and invariants |
| T1 | Analytical | Validate against mathematical solutions |
| T2 | Operator consistency | Validate discrete mathematical relationships |
| T3 | End-to-end reconstruction | Validate complete reconstruction behavior |
| T4 | Independent reference | Validate against independent implementations |

Each level has a different validation strategy and tolerance policy.

### 3.1 T0 — Unit and Structural Tests

Validate individual components independently.

Test areas:

- Image and projection data types.
- Image dimensions and geometry.
- Sinogram dimensions and indexing.
- Memory layout and storage order.
- Input validation.
- Geometry compatibility.
- Measurement semantics.
- Invalid and missing metadata.
- Ownership and lifetime behavior.

Structural properties must be checked exactly.

Example:

```cpp
REQUIRE(image.width() == 256);
REQUIRE(image.height() == 256);

REQUIRE(sinogram.viewCount() == 180);
REQUIRE(sinogram.detectorCount() == 256);

REQUIRE(sinogram.index(2, 3) == 2 * 256 + 3);
```

Floating-point metadata must use approximate comparisons where appropriate.

### 3.2 T1 — Analytical Tests

Validate numerical operations against known mathematical solutions.

Prefer simple geometries for which expected results can be derived independently.

Example: Constant attenuation field.

For a ray passing through a constant-valued region:

$$

p = \int \mu(x,y)\,dl

$$

If the attenuation-like value is constant:

$$

\mu(x,y)=\mu_0

$$

and the ray travels a distance $L$ inside the region:

$$

p_{\mathrm{expected}}=\mu_0L

$$

For:

$$

\mu_0=2,\qquad L=5

$$

the expected projection is:

$$

p_{\mathrm{expected}}=10

$$

The implementation must produce a result consistent with the declared discretization model.

Test cases should include:

- Constant fields.
- Horizontal and vertical rays.
- Axis-aligned geometries.
- Symmetric geometries.
- Boundary intersections.
- Rays that do not intersect the image.
- Simple analytically solvable phantoms.

The expected solution must clearly state whether it is exact for the discrete model or an approximation of the continuous model.

### 3.3 T2 — Operator Consistency Tests

Validate the mathematical relationship between the discrete forward operator and its adjoint.

Let:

$$

A:\mathcal X\rightarrow\mathcal Y

$$

represent the forward projection operator, and:

$$

A^*:\mathcal Y\rightarrow\mathcal X

$$

represent its mathematical adjoint.

The adjointness property is:

$$

\langle Ax,y\rangle_{\mathcal Y}
=
\langle x,A^*y\rangle_{\mathcal X}

$$

For numerical implementations:

$$

\langle Ax,y\rangle_{\mathcal Y}
\approx
\langle x,A^*y\rangle_{\mathcal X}

$$

The test must use the inner products and weighting conventions defined in DOC-05.

#### 3.3.1 Normalized Adjointness Error

Define:

$$

\epsilon_{\mathrm{adj}} =
\frac{
|\langle Ax,y\rangle-\langle x,A^*y\rangle|
}{
\max(
|\langle Ax,y\rangle|,
|\langle x,A^*y\rangle|,
S_{\min}
)
}

$$

Where $S_{\min}$ is a small positive numerical scale safeguard.

The denominator prevents unstable relative comparisons when the inner products are close to zero.

Also calculate the absolute adjointness residual:

$$

E_{\mathrm{adj}} =
|\langle Ax,y\rangle-\langle x,A^*y\rangle|

$$

Both values should be reported.

The normalized error is the primary comparison when its scale is sufficiently large. When both inner products are near zero, use an explicitly defined absolute residual criterion.

#### 3.3.2 Test Procedure

1. Generate a deterministic image vector $x$.
2. Generate a deterministic projection vector $y$.
3. Compute $Ax$.
4. Compute $A^*y$.
5. Calculate both inner products.
6. Calculate absolute and normalized adjointness errors.
7. Compare against the T2 tolerance profile.

Use multiple geometries, vector magnitudes, and random test cases.

**Important:** Adjointness does not independently establish physical correctness. Two incorrect operators can still satisfy the adjoint relationship.

Analytical tests are therefore mandatory.

### 3.4 T3 — End-to-End Reconstruction Tests

Validate the complete synthetic reconstruction pipeline.

```text
Synthetic Phantom
       |
       v
Forward Projection
       |
       v
Sinogram
       |
       v
Projection Filtering
       |
       v
Filtered Sinogram
       |
       v
Backprojection
       |
       v
Reconstructed Image
       |
       v
Numerical Validation
```

The reconstructed image must be compared with the original phantom.

Initial phantom fixtures:

| Phantom | Purpose |
|---|---|
| Constant image | Basic intensity behavior |
| Single-pixel impulse | Indexing and spatial response |
| Uniform square | Analytical projection behavior |
| Circular phantom | Basic reconstruction quality |
| Asymmetric phantom | Detect orientation and axis-order errors |
| Multiple objects | Detect interaction and reconstruction artifacts |

Validation metrics:

- RMSE.
- NRMSE.
- PSNR.
- SSIM.

No single metric is sufficient to establish reconstruction correctness.

Each benchmark must document its phantom, geometry, sampling, filtering configuration, reconstruction parameters, and expected numerical behavior.

### 3.5 T4 — Independent Reference Tests

Compare the implementation against an independently developed reference.

Possible references:

- Analytical projection solutions.
- Independently implemented numerical integration.
- A separate reference forward projector.
- Established CT reconstruction implementations.
- Independently generated synthetic projection fixtures.

The reference must not simply reuse the implementation under test.

Before comparison, ensure that both implementations use equivalent:

- Coordinate systems.
- Geometry.
- Units.
- Detector conventions.
- Sampling assumptions.
- Filtering definitions.
- Interpolation conventions.
- Boundary handling.
- Normalization.

Differences in mathematical conventions must be resolved before interpreting numerical discrepancies.

## 4. Numerical Tolerance Policy

Numerical validation must account for floating-point arithmetic, discretization, and algorithmic approximation.

A single global tolerance is insufficient for all reconstruction tests.

This project adopts a **tiered tolerance policy**, where each testing level has its own tolerance expectations.

### 4.1 Absolute and Relative Tolerance

For two floating-point values $a$ and $b$, consider them approximately equal when:

$$

|a-b|\leq \mathrm{atol}+\mathrm{rtol}|b|

$$

Where:

- `atol`: Absolute tolerance, important when expected values are close to zero.
- `rtol`: Relative tolerance, important when expected values have different magnitudes.
- $b$: Expected reference value.

Both tolerances are necessary.

Using only relative tolerance can produce unstable comparisons near zero. Using only absolute tolerance can make comparisons inappropriate for values with substantially different magnitudes.

### 4.2 Initial Tolerance Profiles

The following values are initial engineering targets for a double-precision implementation.

| Tier | Test category | Initial tolerance |
|---|---|---|
| T0 | Structural and exact properties | Exact comparison where appropriate |
| T1 | Analytical tests | `rtol = 1e-10`, `atol = 1e-12` |
| T2 | Operator adjointness | `rtol = 1e-10`, `atol = 1e-12` |
| T3 | End-to-end reconstruction | Scenario-specific |
| T4 | Independent reference | Reference-specific |

These values are starting targets, not universal guarantees.

T1 and T2 targets must be verified against the actual projector discretization, geometry, and floating-point accumulation strategy.

T3 and T4 must not inherit arbitrary tolerances from T1 or T2.

### 4.3 T0 — Exact and Structural Comparisons

Use exact comparisons for properties that should not involve numerical approximation.

Examples:

- Image width and height.
- Number of projection views.
- Number of detector elements.
- Array indexing.
- Storage ordering.
- Enumeration values.
- Validity flags.
- Expected object ownership behavior.

For floating-point metadata, use approximate comparison only when the contract permits numerical representation differences.

### 4.4 T1 — Analytical Comparison

Use the initial double-precision profile:

```cpp
constexpr double kAnalyticalRtol = 1e-10;
constexpr double kAnalyticalAtol = 1e-12;
```

Example:

```cpp
const double expected = 10.0;
const double actual = forwardProjection(...);

const double error = std::abs(actual - expected);

REQUIRE(
    error <= kAnalyticalAtol +
             kAnalyticalRtol * std::abs(expected)
);
```

Analytical fixtures should preferably use simple values and geometries that minimize ambiguity.

If a test includes numerical integration or approximate boundary calculations, its tolerance must account for those approximations explicitly.

Do not increase tolerance merely because a test fails. First investigate whether the failure indicates an implementation defect, a mistaken analytical expectation, or an unsuitable tolerance.

### 4.5 T2 — Adjointness Comparison

Use the initial double-precision profile:

```cpp
constexpr double kAdjointRtol = 1e-10;
constexpr double kAdjointAtol = 1e-12;
```

Evaluate the absolute residual and normalized adjointness error defined in Section 3.3.

The test should include:

- Multiple deterministic random vectors.
- Different image and detector dimensions.
- Multiple projection angles.
- Different valid geometries.
- Nonuniform weighting where supported by the operator contract.

The initial target must be experimentally validated. If the chosen discretization or accumulation method cannot reliably meet it, document the measured behavior and revise the profile with mathematical and numerical justification.

### 4.6 T3 — Reconstruction-Specific Tolerances

End-to-end reconstruction does not have one universal floating-point tolerance.

Its numerical behavior depends on:

- Phantom geometry.
- Number of projection views.
- Detector sampling.
- Reconstruction algorithm.
- Filtering method.
- Interpolation.
- Boundary treatment.
- Noise assumptions.
- Image normalization.

Therefore, each reconstruction fixture must define its own expected metric ranges.

Example fixture specification:

```yaml
fixture: circular_phantom
precision: float64

geometry:
  image_size: [256, 256]
  projection_views: 180
  detector_count: 256

metrics:
  rmse:
    maximum: "set from validated reference reconstruction"
  nrmse:
    maximum: "set from validated reference reconstruction"
  psnr:
    minimum: "set from validated reference reconstruction"
  ssim:
    minimum: "set from validated reference reconstruction"
```

These thresholds must be established from validated reference reconstructions and recorded with their acquisition configuration, not guessed or copied from unrelated problem instances.

### 4.7 T4 — Reference-Specific Tolerances

Reference comparisons must account for the numerical properties of both implementations.

Document:

- Reference implementation and version.
- Reference precision.
- Input geometry and conventions.
- Numerical integration method.
- Interpolation and boundary behavior.
- Expected numerical error.
- Comparison metric and tolerance.

If the reference implementation itself is approximate, its error must be considered when establishing the acceptance threshold.

### 4.8 Tolerance Override Rules

A test may override its default tier tolerance only when there is a documented reason.

Acceptable reasons include:

- Known numerical integration error.
- Different accumulation order.
- Approximate interpolation.
- Reference implementation limitations.
- A mathematically justified discretization difference.

Every override must explain:

1. Why the default tolerance is unsuitable.
2. What numerical behavior causes the difference.
3. Why the selected tolerance is sufficient.
4. What independent validation prevents the tolerance from hiding defects.

**Prohibited practice:** Increasing tolerance solely to make a failing test pass.

### 4.9 Precision Baseline

The initial reconstruction implementation will use `double` precision.

Reasons:

- Better numerical headroom during early algorithm development.
- Reduced sensitivity to accumulation error.
- More reliable analytical and adjointness validation.
- A useful baseline for future performance comparisons.

Single-precision support can be introduced later with separately validated tolerance profiles.

Do not assume that double-precision tolerances are appropriate for single-precision implementations.

## 5. Reconstruction Quality Metrics

End-to-end reconstruction tests must evaluate numerical similarity between the reconstructed image and its reference phantom.

Visual inspection alone is insufficient.

The project initially adopts four metrics:

1. Root Mean Square Error (RMSE).
2. Normalized Root Mean Square Error (NRMSE).
3. Peak Signal-to-Noise Ratio (PSNR).
4. Structural Similarity Index (SSIM).

Each metric captures a different aspect of reconstruction quality.

### 5.1 Reference and Reconstructed Images

Let:

- $I$: Reference image.
- $\hat I$: Reconstructed image.
- $N$: Total number of pixels.
- $I_i$: Reference intensity at pixel $i$.
- $\hat I_i$: Reconstructed intensity at pixel $i$.

Both images must have:

- Identical dimensions.
- Identical spatial orientation.
- Identical pixel correspondence.
- Compatible intensity units.
- Explicitly documented normalization.

Metrics must not be computed before these conditions are satisfied.

### 5.2 Root Mean Square Error (RMSE)

RMSE measures the average magnitude of pixel-wise reconstruction error.

$$

\mathrm{RMSE} =
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
(I_i-\hat I_i)^2
}

$$

**Interpretation:**

- Lower RMSE indicates smaller pixel-wise error.
- Zero RMSE indicates exact numerical agreement.
- RMSE is expressed in the same intensity units as the input images.
- Large local errors have a stronger influence because the differences are squared.

Example:

Reference:

$$

I=[1,2,3,4]

$$

Reconstruction:

$$

\hat I=[1,2,2,4]

$$

Then:

$$

\mathrm{RMSE}
=
\sqrt{\frac{0^2+0^2+1^2+0^2}{4}}
=0.5

$$

RMSE will be the primary pixel-wise error metric for initial reconstruction tests.

### 5.3 Normalized Root Mean Square Error (NRMSE)

RMSE depends on the intensity scale of the image.

NRMSE provides a normalized measure.

This project initially adopts reference-range normalization:

$$

\mathrm{NRMSE} =
\frac{\mathrm{RMSE}}
{I_{\max}-I_{\min}}

$$

Where:

- $I_{\max}$: Maximum reference intensity.
- $I_{\min}$: Minimum reference intensity.

**Interpretation:**

- Lower NRMSE indicates better reconstruction.
- Zero indicates exact agreement.
- The result is dimensionless.
- The normalization range must come from the reference image, not the reconstructed image.

**Edge case:** If the reference image has zero intensity range, NRMSE is undefined. The test must report this condition rather than silently dividing by zero.

Other NRMSE conventions exist, including normalization by reference RMS or Euclidean norm. They must not be mixed with this project's reference-range definition.

### 5.4 Peak Signal-to-Noise Ratio (PSNR)

PSNR expresses reconstruction error relative to a defined reference signal range.

$$

\mathrm{PSNR} =
20\log_{10}
\left(
\frac{R}{\mathrm{RMSE}}
\right)

$$

Where:

$$

R=I_{\max}-I_{\min}

$$

**Interpretation:**

- Higher PSNR indicates better reconstruction.
- Lower RMSE produces higher PSNR.
- PSNR is expressed in decibels (dB).
- If RMSE is zero and $R>0$, PSNR is positive infinity.
- If $R=0$, this definition is undefined.

The reference range must be explicitly recorded for every fixture.

Do not use the maximum representable value of a storage type as $R$ unless that is explicitly the intended signal range.

### 5.5 Structural Similarity Index (SSIM)

RMSE and PSNR measure pixel-wise differences but do not adequately describe structural similarity.

SSIM compares local luminance, contrast, and structural information.

For two local image regions $x$ and $y$:

$$

\mathrm{SSIM}(x,y)=
\frac{
(2\mu_x\mu_y+C_1)
(2\sigma_{xy}+C_2)
}{
(\mu_x^2+\mu_y^2+C_1)
(\sigma_x^2+\sigma_y^2+C_2)
}

$$

Where:

- $\mu_x,\mu_y$: Local means.
- $\sigma_x^2,\sigma_y^2$: Local variances.
- $\sigma_{xy}$: Local covariance.
- $C_1,C_2$: Stabilizing constants.

A typical definition uses:

$$

C_1=(K_1L)^2

$$

$$

C_2=(K_2L)^2

$$

Where $L$ is the explicitly specified dynamic range and $K_1,K_2$ are documented constants.

SSIM implementation must document:

- Window size and shape.
- Gaussian or uniform window weighting.
- Boundary handling.
- Dynamic range $L$.
- Constants $K_1,K_2$.
- Local averaging method.
- Whether the final score is averaged over the entire image or a specified region.

**Interpretation:**

- Higher SSIM generally indicates greater structural similarity.
- A score near 1 indicates strong similarity.
- SSIM must not be treated as a substitute for numerical error measurements.

### 5.6 Metric Selection by Test Objective

| Test objective | Primary metric | Supporting metric |
|---|---|---|
| Pixel-wise numerical accuracy | RMSE | NRMSE |
| Scale-normalized error | NRMSE | RMSE |
| Signal-range-relative error | PSNR | RMSE |
| Structural preservation | SSIM | RMSE |
| Complete phantom reconstruction | RMSE | NRMSE, PSNR, SSIM |

No single metric should determine whether a reconstruction is correct.

For example, an image may have a relatively good SSIM while containing intensity bias. Conversely, a small pixel-wise error may coexist with localized structural artifacts.

### 5.7 Metric Validation Tests

The metric implementation itself must be tested independently.

Required cases:

| Case | Expected behavior |
|---|---|
| Identical images | RMSE = 0, NRMSE = 0, PSNR = infinity |
| One pixel differs | Positive RMSE |
| Constant nonzero reference | NRMSE and PSNR undefined under range normalization |
| Increased reconstruction error | RMSE increases |
| Identical structural content | SSIM approaches 1 |
| Different image dimensions | Validation failure |
| NaN or infinite input | Explicit rejection or documented invalid result |

Metric implementations must reject incompatible image dimensions and handle non-finite values explicitly.

### 5.8 Acceptance Thresholds

Do not introduce universal RMSE, NRMSE, PSNR, or SSIM acceptance thresholds at this stage.

Thresholds depend on the reconstruction algorithm and experimental configuration.

For each synthetic phantom fixture:

1. Establish a validated reference reconstruction.
2. Record the acquisition and reconstruction configuration.
3. Calculate all four metrics.
4. Define acceptable ranges.
5. Explain the rationale for each threshold.
6. Keep the fixture deterministic.

Thresholds should be introduced incrementally as the reconstruction pipeline becomes available.

**Important:** A visually plausible reconstruction is not sufficient evidence of numerical correctness.

## 6. Deterministic Testing and Reproducibility

Numerical tests must be reproducible across repeated executions.

A failing test must provide enough information to reproduce the same input conditions and investigate the failure.

This section defines the project's deterministic testing strategy.

### 6.1 Deterministic Random Number Generation

Randomized tests are useful for validating operators across a broad range of inputs.

However, random inputs must be generated using fixed seeds.

Use a fixed project-level seed initially:

```cpp
constexpr std::uint32_t kTestSeed = 0x5EED1234;
```

Example using the C++ standard library:

```cpp
#include <cstdint>
#include <random>

constexpr std::uint32_t kTestSeed = 0x5EED1234;

std::mt19937 generator{kTestSeed};

std::uniform_real_distribution<double> distribution{
    -1.0,
    1.0
};

const double value = distribution(generator);
```

**Rules:**

- Never use an unseeded random generator in correctness tests.
- Record the seed when a randomized test fails.
- Use separate generators for logically independent test cases.
- Avoid dependence on execution order.
- Do not share mutable random-generator state across parallel tests.
- Keep deterministic fixtures for important regression scenarios.

For reproducibility across different standard-library implementations, do not assume that `std::uniform_real_distribution` produces identical sequences everywhere. If cross-platform bitwise-identical inputs are required, use a project-defined deterministic generator and mapping strategy.

### 6.2 Standard Phantom Fixtures

Maintain a small collection of reusable synthetic phantoms.

| Fixture | Purpose |
|---|---|
| Constant phantom | Validate intensity and basic projection behavior |
| Impulse phantom | Validate spatial response and indexing |
| Square phantom | Validate simple geometry and boundaries |
| Circular phantom | Validate basic reconstruction behavior |
| Asymmetric phantom | Detect axis swaps, flips, and orientation errors |
| Multi-object phantom | Validate multiple structures and spatial interactions |
| Deterministic random phantom | Exercise numerical behavior across varied inputs |

Each fixture must have:

- Fixed dimensions.
- Fixed geometry.
- Fixed intensity values.
- Defined coordinate conventions.
- Explicit expected properties.
- Stable generation logic.

Fixtures should be generated programmatically when practical. This avoids unnecessary binary test assets and makes their mathematical definitions inspectable.

### 6.3 Reference Data Management

Reference data must be treated as versioned test assets.

For each reference fixture, record:

- Fixture identifier.
- Generation method.
- Source implementation or mathematical derivation.
- Image dimensions.
- Geometry.
- Precision.
- Units.
- Expected metrics or numerical values.
- Relevant algorithm configuration.

A possible organization:

```text
tests/
└── data/
    ├── phantoms/
    │   ├── constant/
    │   ├── impulse/
    │   ├── square/
    │   └── asymmetric/
    │
    └── reference/
        ├── analytical/
        └── independent/
```

Do not overwrite reference fixtures simply because the current implementation produces different results.

A reference update must include an explanation of why the previous expected result was incorrect or why the mathematical contract changed.

### 6.4 Regression Testing

Every discovered numerical defect should result in a regression test.

A regression test must:

1. Reproduce the original failure.
2. Use deterministic input data.
3. Identify the violated mathematical or software contract.
4. Fail before the correction.
5. Pass after the correction.
6. Remain part of the permanent test suite.

Examples:

- Incorrect detector indexing.
- Reversed projection-view ordering.
- Incorrect image orientation.
- Missing geometric scaling.
- Adjointness violation.
- Incorrect boundary interpolation.
- Numerical instability for degenerate geometry.

Regression tests must validate the underlying property rather than merely reproducing one specific output.

### 6.5 Cross-Platform Reproducibility

The project should aim for consistent numerical behavior across supported compilers and platforms.

However, bitwise-identical floating-point results are not a general requirement.

Results may differ because of:

- Floating-point instruction selection.
- Compiler optimizations.
- Fused multiply-add operations.
- Floating-point accumulation order.
- Math-library implementations.
- Parallel execution order.

The correctness suite should therefore use justified numerical tolerances instead of requiring bitwise equality for floating-point algorithms.

Exact equality remains appropriate for structural properties and deterministic integer operations.

### 6.6 Test Failure Diagnostics

A numerical test failure must report enough information to identify its cause.

Recommended diagnostic information:

```text
Test:
  test_forward_projection_constant_field

Tier:
  T1 - Analytical

Expected:
  10.0

Actual:
  9.9999999998

Absolute error:
  2.0e-10

Relative error:
  2.0e-11

Tolerance:
  rtol = 1.0e-10
  atol = 1.0e-12

Seed:
  0x5EED1234

Geometry:
  Image = 256 x 256
  Views = 180
  Detectors = 256
```

For randomized tests, report the seed and test-case identifier.

For operator tests, also report both inner products, the absolute residual, and the normalized adjointness error.

### 6.7 Environment and Configuration Recording

For reference-sensitive tests, record relevant build and runtime information:

- Compiler and version.
- Operating system.
- Build configuration.
- Floating-point precision.
- Relevant compiler flags.
- Reconstruction algorithm configuration.
- Test fixture identifier.
- Random seed, when applicable.

This information helps distinguish an algorithmic regression from an environment-dependent numerical difference.

### 6.8 Reproducibility Principle

The same test configuration should produce the same logical test inputs and expected mathematical properties.

Numerical outputs may differ within the documented tolerance profile.

A test must never silently adapt its expected result to the implementation being tested.

The reference defines correctness; the implementation is evaluated against it.

## 7. Test-Driven Development Workflow

The reconstruction module must follow a Red-Green-Refactor workflow.

Tests are written before or alongside the implementation of each mathematical component.

### 7.1 RED — Define the Expected Behavior

Before implementing a component:

1. Identify the mathematical or software contract.
2. Define the expected input and output.
3. Select the appropriate testing tier.
4. Construct deterministic test data.
5. Define the expected result independently.
6. Write a test that initially fails.

Example:

For a constant attenuation field, define the expected line integral analytically before implementing the forward projector.

### 7.2 GREEN — Implement the Minimum Functionality

Implement the minimum functionality required to satisfy the failing test.

Requirements:

- Preserve the mathematical contract.
- Avoid unnecessary abstractions.
- Do not weaken assertions.
- Do not increase tolerances simply to obtain a passing test.
- Run the relevant test and verify the result.

### 7.3 REFACTOR — Improve the Implementation

Once the test passes:

- Improve implementation structure.
- Remove duplicated logic.
- Improve numerical stability where necessary.
- Review memory allocation and data ownership.
- Add relevant edge-case tests.
- Run the complete test suite.

Refactoring must preserve previously established behavior.

### 7.4 Testing Order

Develop and validate reconstruction components in the following order:

1. Data structures and geometry.
2. Sinogram layout and measurement semantics.
3. Forward projection.
4. Adjoint projection.
5. Adjointness validation.
6. Filtering and reconstruction components.
7. End-to-end phantom reconstruction.
8. Independent reference comparisons.

This order follows the mathematical dependencies and reduces the difficulty of diagnosing failures.

## 8. Test Architecture

The project already uses Catch2 v3.5.2 through CMake FetchContent.

The existing test executable is `ir_tests`, with tests registered through `catch_discover_tests`.

The reconstruction test suite should extend this infrastructure rather than introduce another testing framework.

### 8.1 Proposed Directory Structure

```text
tests/
├── unit/
│   ├── test_image_geometry.cpp
│   ├── test_sinogram_layout.cpp
│   └── test_reconstruction_data.cpp
│
├── analytical/
│   ├── test_forward_projection.cpp
│   └── test_backprojection.cpp
│
├── operators/
│   └── test_adjointness.cpp
│
├── reconstruction/
│   ├── test_fbp_phantom.cpp
│   └── test_reconstruction_metrics.cpp
│
├── reference/
│   └── test_reference_projections.cpp
│
├── support/
│   ├── numeric_tolerance.hpp
│   ├── numeric_assertions.hpp
│   ├── phantom_fixtures.hpp
│   └── deterministic_random.hpp
│
└── data/
    ├── phantoms/
    └── reference/
```

This is a proposed logical organization.

Do not move existing tests or reorganize the repository unnecessarily. Introduce directories incrementally as reconstruction functionality is implemented.

### 8.2 Centralized Numerical Test Support

Tolerance definitions should be centralized in the test-support layer.

Example:

```cpp
struct NumericTolerance
{
    double absolute;
    double relative;
};

namespace tolerance
{
    constexpr NumericTolerance analytical{
        1e-12,
        1e-10
    };

    constexpr NumericTolerance adjointness{
        1e-12,
        1e-10
    };
}
```

A generic comparison helper can implement:

$$

|a-b|\leq
\mathrm{atol}+\mathrm{rtol}|b|

$$

The helper should also check for non-finite values and provide useful failure diagnostics.

Keep these helpers inside the test-support layer.

They should not become part of the production library API unless a genuine production requirement emerges.

### 8.3 Test Naming Convention

Use descriptive names that identify the operation and expected behavior.

Examples:

```text
ForwardProjection_ConstantField_ReturnsExpectedIntegral

ForwardProjection_NonIntersectingRay_ReturnsZero

ForwardProjection_InvalidGeometry_RejectsInput

Adjointness_ParallelBeamGeometry_SatisfiesInnerProductIdentity

Reconstruction_CircularPhantom_MeetsReferenceMetrics

Metrics_IdenticalImages_ReturnsZeroRmse
```

Test names should communicate the expected behavior without requiring the reader to inspect the implementation.

## 9. Continuous Integration Strategy

All correctness tests must run as part of the existing CI pipeline.

Initial CI responsibilities:

- Configure the project with tests enabled.
- Build the library and test executable.
- Execute the Catch2 test suite.
- Report failing tests.
- Preserve test failure diagnostics.

The existing Windows CI configuration should be retained.

Additional supported operating systems and compilers can be introduced as the project expands.

### 9.1 CI Test Categories

| Category | CI execution |
|---|---|
| T0 — Structural | Every relevant build |
| T1 — Analytical | Every relevant build |
| T2 — Adjointness | Every relevant build |
| T3 — Reconstruction | Every relevant build once implemented |
| T4 — Independent reference | Every relevant build when practical |
| Performance benchmarks | Separate benchmark job |

Correctness tests must not depend on benchmark timing.

A benchmark failure should not be confused with a mathematical correctness failure.

## 10. Performance and Memory Validation

Performance measurement is important for image reconstruction, but it must remain separate from numerical correctness.

Initially, performance tests should collect measurements without enforcing strict timing thresholds.

Measure:

- Forward projection execution time.
- Adjoint projection execution time.
- Reconstruction execution time.
- Peak memory usage.
- Temporary allocation volume, where measurable.
- Scaling with image dimensions.
- Scaling with projection views and detector count.

Record the hardware, compiler, build configuration, and test dimensions.

Performance regression thresholds may be introduced after stable baseline measurements exist.

Avoid strict wall-clock assertions in ordinary correctness tests because machine load and execution environments can make them unreliable.

## 11. Acceptance Criteria

The reconstruction testing strategy is considered established when the following requirements are documented and implemented to the extent supported by the current project stage.

### 11.1 Testing Methodology

- [ ] T0–T4 testing levels are defined.
- [ ] Each level has a clear validation objective.
- [ ] Red-Green-Refactor workflow is documented.
- [ ] Tests validate mathematical contracts rather than implementation details.

### 11.2 Numerical Validation

- [ ] Absolute and relative tolerance definitions are documented.
- [ ] Initial double-precision tolerance profiles are specified.
- [ ] Tolerance overrides require justification.
- [ ] Adjointness residual and normalized error are defined.
- [ ] Floating-point, discretization, and algorithmic errors are distinguished.

### 11.3 Reconstruction Metrics

- [ ] RMSE definition is documented.
- [ ] NRMSE reference-range normalization is documented.
- [ ] PSNR dynamic-range definition is documented.
- [ ] SSIM configuration requirements are documented.
- [ ] Metric edge cases are specified.
- [ ] Universal reconstruction thresholds are avoided until validated fixtures exist.

### 11.4 Determinism

- [ ] Fixed random seeds are used.
- [ ] Standard phantom fixtures are defined.
- [ ] Reference data provenance is recorded.
- [ ] Regression tests are deterministic.
- [ ] Numerical failure diagnostics are specified.

### 11.5 Infrastructure

- [ ] Catch2 is used consistently with existing infrastructure.
- [ ] Test-support organization is documented.
- [ ] Correctness tests run in CI.
- [ ] Performance benchmarks are separated from correctness tests.

## 12. Dependencies and Design Boundaries

This document defines the testing strategy. It does not redefine the mathematical contracts established by other architecture documents.

The following dependencies must remain authoritative:

| Document | Responsibility |
|---|---|
| DOC-03 — Reconstruction Data Model | Image, geometry, measurement semantics, and data layout |
| DOC-05 — Projection Operator Contracts | Forward operator, adjoint operator, and inner-product definitions |
| DOC-06 — Reconstruction Testing Strategy | Validation methodology, tolerances, metrics, and reproducibility |

### 12.1 Decisions Deferred to Mathematical Contracts

The following details must be inherited from the corresponding architecture documents rather than independently invented here:

- Exact forward-projector discretization.
- Adjoint inner-product weighting.
- Detector and image coordinate conventions.
- Boundary and interpolation conventions.
- Measurement units and normalization.

If these contracts change, affected analytical fixtures and reference tests must be reviewed.

### 12.2 Future Extensions

The current strategy targets 2D reconstruction using double precision.

Future extensions may introduce:

- Single-precision numerical profiles.
- GPU-based reconstruction validation.
- 3D reconstruction and volumetric phantoms.
- Parallel and distributed execution.
- Noise and statistical reconstruction tests.
- Performance regression thresholds.

These extensions should preserve the testing principles established here without prematurely introducing generic abstractions.

## 13. Final Design Decisions

The following decisions form the baseline for reconstruction testing:

1. Adopt five validation tiers: T0–T4.
2. Use absolute and relative tolerances together.
3. Use double precision as the initial numerical baseline.
4. Validate simple mathematical cases before complete reconstruction.
5. Test adjointness independently from physical correctness.
6. Use RMSE, NRMSE, PSNR, and SSIM for complementary reconstruction evaluation.
7. Use deterministic random inputs and reusable synthetic phantoms.
8. Centralize tolerance profiles in test-support code.
9. Require documented justification for tolerance overrides.
10. Keep performance benchmarks separate from correctness tests.
11. Extend the existing Catch2 and CI infrastructure.
12. Defer mathematical details to DOC-03 and DOC-05.

The objective is not merely to make reconstruction tests pass. It is to establish a reproducible numerical validation framework that provides evidence that the implementation satisfies its mathematical and software contracts.
