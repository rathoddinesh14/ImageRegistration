# ImageRegistration

Modern **C++20** library for **2D image registration**, designed to stay modular and extensible (including a future path to 3D and time-series).

| | |
|---|---|
| **License** | MIT |
| **Language** | C++20 |
| **Core dependency** | [Eigen](https://eigen.tuxfamily.org/) 3.3+ |
| **Tests** | [Catch2](https://github.com/catchorg/Catch2) (optional) |
| **CI** | GitHub Actions (Windows build/test, clang-format, clang-tidy, API docs) |

## What this library is

A **composition-friendly** registration stack:

- Small **value types** and pure interfaces first  
- Algorithms plugged in later (metrics, optimizers, transforms)  
- Core stays free of I/O and heavy frameworks  

```text
Fixed image + moving image
        │
        ▼
  transform + interpolator + resampler + metric (+ optimizer)
        │
        ▼
  RegistrationResult (success, metric, iterations, …)
```

## Current status

| Area | Status |
|------|--------|
| Build / CI / coding standards | Done |
| Coordinate conventions | [Documented](docs/architecture/coordinate-conventions.md) |
| `Point2D`, `ImageGeometry2D`, `Image2D` | Done |
| `Transform2D`, `Translation2D`, `Rigid2D` | Done |
| `Interpolator2D`, nearest-neighbor, bilinear | Done |
| `Metric2D`, `MeanSquaresMetric2D` | Done |
| `Resampler2D`, `ImageResampler2D` | Done |
| Optional PNG write (`ir::io`) | Done |
| Synthetic translation example | Done |
| `RegistrationResult`, `ErrorCode` | Done |
| Optimizer / full registration method | Planned (see `docs/plan/`) |
| 3D / time-series | Future |

## Mathematical formulation (implemented)

Notation uses continuous **index** coordinates $(i, j)$ and **physical** coordinates
$(x, y)$. Integer lattice sites are **pixel centers**. See
[coordinate conventions](docs/architecture/coordinate-conventions.md).

### Geometry

For width $W$, height $H$, spacing $\mathbf{s} = (s_i, s_j)$, origin $\mathbf{o}$
(physical location of index $(0,0)$), and $2 \times 2$ direction matrix $\mathbf{D}$
(default identity).

**Index to physical:**

$$
\mathbf{p} = \mathbf{o} + \mathbf{D}
\begin{pmatrix}
s_i \, i \\
s_j \, j
\end{pmatrix}
$$

**Physical to continuous index:**

$$
\begin{pmatrix}
i \\
j
\end{pmatrix}
=
\begin{pmatrix}
1/s_i & 0 \\
0 & 1/s_j
\end{pmatrix}
\mathbf{D}^{-1} (\mathbf{p} - \mathbf{o})
$$

`Image2D` stores intensities $I(i,j)$ in **row-major** order:

$$
\mathrm{linearIndex} = j \, W + i, \quad i \in \{0,\ldots,W-1\}, \; j \in \{0,\ldots,H-1\}
$$

### Transforms (`Transform2D`)

A transform maps a physical point $\mathbf{p}$ to $\mathbf{p}' = T(\mathbf{p})$.

**Translation2D**

$$
\mathbf{p}' = \mathbf{p} + \mathbf{t}
$$

**Rigid2D** (rotation then translation about the physical origin)

$$
\mathbf{R}(\theta) =
\begin{pmatrix}
\cos\theta & -\sin\theta \\
\sin\theta & \cos\theta
\end{pmatrix},
\qquad
\mathbf{p}' = \mathbf{R}(\theta) \, \mathbf{p} + \mathbf{t}
$$

For resampling, $T$ maps **fixed physical → moving physical**.

### Interpolation (`Interpolator2D`)

Sample continuous index $\mathbf{u} = (u, v)$ on image $I$.

**Nearest neighbor**

$$
I_{\mathrm{NN}}(\mathbf{u}) = I\bigl(\mathrm{round}(u),\, \mathrm{round}(v)\bigr)
$$

Out-of-bounds samples use `BoundsPolicy` (constant fill or clamp to the lattice).

**Bilinear** (`LinearInterpolator2D`)

Let $i_0 = \lfloor u \rfloor$, $j_0 = \lfloor v \rfloor$, $f_x = u - i_0$,
$f_y = v - j_0$, and $I_{00}, I_{10}, I_{01}, I_{11}$ the four corner samples:

$$
\begin{aligned}
I_{\mathrm{lin}}(\mathbf{u})
&= (1-f_x)(1-f_y)\, I_{00}
 + f_x(1-f_y)\, I_{10} \\
&\quad + (1-f_x)f_y\, I_{01}
 + f_x f_y\, I_{11}
\end{aligned}
$$

### Resampling (`ImageResampler2D`)

Build output $I_{\mathrm{out}}$ on fixed geometry $G_f$. For each integer fixed
index $(i,j)$:

$$
\begin{aligned}
\mathbf{p}_f &= \mathrm{indexToPhysical}_{G_f}(i, j) \\
\mathbf{p}_m &= T(\mathbf{p}_f) \\
\mathbf{u}_m &= \mathrm{physicalToIndex}_{G_m}(\mathbf{p}_m) \\
I_{\mathrm{out}}(i,j) &= \mathrm{Interp}(I_m,\, \mathbf{u}_m)
\end{aligned}
$$

This is **inverse (pull) warping**: iterate the fixed grid, sample the moving image.

### Metric (`MeanSquaresMetric2D`)

For same-size images $I_f$ and $I_m$ with $N = W H$ pixels:

$$
\mathrm{MSE}(I_f, I_m)
= \frac{1}{N} \sum_{i,j} \bigl( I_f(i,j) - I_m(i,j) \bigr)^2
$$

`isMinimize()` is true (lower is better).

### Registration result

`RegistrationResult` is a **solve summary** (not a cost functional): success flag,
`ErrorCode`, final metric value, iteration count, optional message. Full registration
optimization is still planned.

## Architecture

| Component | Role |
|-----------|------|
| **core** | Points, image geometry, images, `ErrorCode` |
| **transform** | Translation, rigid, … |
| **interpolator** | Sample at continuous indices |
| **metric** | Similarity / dissimilarity |
| **resampler** | Warp moving image onto fixed geometry |
| **registration** | `RegistrationResult` (method/orchestrator planned) |
| **io** | Optional export (PNG) |
| **optimizer** | Parameter search (planned) |

Public API: `include/ir/`. Tests: `tests/`. Plan and ADRs: `docs/plan/`.

## Requirements

- CMake ≥ 3.16  
- C++20 compiler (MSVC on Windows is the CI baseline)  
- Eigen 3.3+ (system install or fetched by CMake)  

## Build

```bash
# Configure with tests (recommended while developing)
cmake -S . -B build -DIR_BUILD_TESTS=ON

# Build
cmake --build build --config Release

# Run tests
ctest --test-dir build -C Release --output-on-failure
```

Optional flags:

| Option | Default | Meaning |
|--------|---------|---------|
| `IR_BUILD_TESTS` | `OFF` | Catch2 unit tests (also enables IO) |
| `IR_BUILD_EXAMPLES` | `OFF` | Examples (also enables IO for PNG output) |
| `IR_BUILD_IO` | `OFF` | Optional PNG write library |
| `IR_BUILD_BENCHMARKS` | `OFF` | Benchmarks (placeholder) |

### Windows (Visual Studio)

Prefer an **x64 Native Tools** prompt (or CMake Tools in VS Code) so MSVC is used—not MinGW/Strawberry on `PATH`.

```powershell
cmake -S . -B build -DIR_BUILD_TESTS=ON -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

### VS Code

This repo includes `.vscode/settings.json` and extension recommendations (C/C++, CMake Tools). Open the **folder**, configure with CMake Tools, then use Go to Definition / Ctrl+click.

## Example: synthetic translation

Demonstrates the pieces that already work end-to-end: synthetic fixed/moving disks, known shift, `ImageResampler2D` + nearest-neighbor, MSE before/after, optional PNG export.

```bash
cmake -S . -B build -DIR_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ir_example_synthetic_translation

# Optional output directory for fixed.png / moving.png / aligned.png
./build/examples/synthetic_translation/ir_example_synthetic_translation ./out
```

Expected console output (approximate):

```text
Synthetic translation demo
  image size: 64 x 64
  known shift (index i): +8
  MSE before resample: ...
  MSE after  resample: 0
  wrote PNGs: yes under ./out
```

Source: `examples/synthetic_translation/main.cpp`.

## Layout

```text
include/ir/          Public headers (core, transform, interpolator, metric, resampler, registration, io)
examples/            Runnable demos (enable with IR_BUILD_EXAMPLES)
tests/               Catch2 tests
docs/plan/           Vision, backlog, ADRs, implemented notes
docs/architecture/   Coordinate conventions
.github/workflows/   CI
```

## Documentation

- [Coding standards](docs/coding-standards.md)
- [Coordinate conventions](docs/architecture/coordinate-conventions.md)
- [Design principles](docs/plan/00-vision/design-principles.md)
- [Backlog](docs/plan/02-backlog/)
- [Decisions (ADRs)](docs/plan/03-decisions/)

## Contributing

1. Prefer **tests-first** for new types and algorithms.  
2. Follow naming: `PascalCase` types, `camelCase` methods, `m_` members, `#pragma once`.  
3. Prefer `/** … */` for multi-line public API docs.  
4. Keep CI green (format, tidy, Windows tests).  

## License

MIT — see [LICENSE](LICENSE).
