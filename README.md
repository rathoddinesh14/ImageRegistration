# ImageRegistration

A clean, modular, extensible 2D image-registration library written in modern C++ (C++20).

## Purpose

Provide a well-structured foundation for 2D image registration algorithms. The design emphasizes separation of concerns, dependency inversion, and composition so that future extensions (including 3D support) can be added without major refactoring of the core.

## Current Scope

- **2D image registration only**
- No algorithm implementations yet — this repository currently contains only the project structure, public headers (declarations), and build scaffolding.

## High-Level Architecture

| Component       | Responsibility                                      |
|-----------------|-----------------------------------------------------|
| `core`          | Fundamental 2D image and geometry concepts          |
| `transform`     | Transformation abstractions (translation, rigid, affine) |
| `interpolator`  | Strategies for evaluating an image at non-integer coordinates |
| `metric`        | Similarity / dissimilarity metric abstractions      |
| `optimizer`     | Optimization abstractions                           |
| `resampler`     | Generation of a transformed / resampled image       |
| `registration`  | High-level registration orchestration               |

Public headers live under `include/ir/`. Implementation files will live under `src/`. Tests mirror the source layout under `tests/`.

## Build Prerequisites

- CMake ≥ 3.16
- A C++20-compliant compiler
- Eigen 3.3+ (linear algebra)

## Basic Build Commands

```bash
# Configure (release build, no tests/examples)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Configure with tests
cmake -S . -B build -DIR_BUILD_TESTS=ON

# Build
cmake --build build

# Install (optional)
cmake --install build
```

## Status

Implementation is intentionally not yet present. The repository currently provides only the directory layout, empty/declaration-only headers, CMake configuration, and documentation placeholders.
