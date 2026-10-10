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
  transform + metric + interpolator + optimizer
        │
        ▼
  RegistrationResult (success, transform, metric value, …)
```

## Current status

| Area | Status |
|------|--------|
| Build / CI / coding standards | In place |
| `Point2D` | Implemented + tested |
| `ImageGeometry2D` | Implemented + tested (this PR) |
| `Image2D`, transforms, metrics, optimizers | Planned (see `docs/plan/`) |
| Full registration pipeline | Not yet |

## Architecture (target)

| Component | Role |
|-----------|------|
| **core** | Points, image geometry, images |
| **transform** | Translation, rigid, affine, … |
| **interpolator** | Sample image at non-integer coordinates |
| **metric** | Similarity / dissimilarity |
| **optimizer** | Parameter search |
| **resampler** | Warp / resample an image |
| **registration** | Orchestrate a full run |

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
| `IR_BUILD_TESTS` | `OFF` | Build Catch2 unit tests |
| `IR_BUILD_EXAMPLES` | `OFF` | Examples (synthetic translation demo) |
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

## Layout

```text
include/ir/          Public headers (core, transform, …)
tests/               Catch2 tests
docs/plan/           Vision, backlog, ADRs, implemented notes
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
