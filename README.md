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
  RegistrationResult (success, transform, metric value, …)
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
| Synthetic translation example | Done (this branch) |
| Optimizer / full registration method | Planned (see `docs/plan/`) |
| 3D / time-series | Future |

## Architecture

| Component | Role |
|-----------|------|
| **core** | Points, image geometry, images |
| **transform** | Translation, rigid, … |
| **interpolator** | Sample at continuous indices |
| **metric** | Similarity / dissimilarity |
| **resampler** | Warp moving image onto fixed geometry |
| **io** | Optional export (PNG) |
| **optimizer** | Parameter search (planned) |
| **registration** | Orchestrate a full run (planned) |

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
include/ir/          Public headers (core, transform, interpolator, metric, resampler, io)
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
