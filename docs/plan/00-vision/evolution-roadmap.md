# Evolution Roadmap

## Phase 1 — Solid 2D Foundation (current)
- Pure interfaces and data structures
- Minimal concrete implementations (Translation, NearestNeighbor, MSE, …)
- Basic registration pipeline
- Tests with Catch2
- CI

## Phase 2 — 3D Spatial Support
- Parallel `*3D` types (`Image3D`, `Point3D`, `Transform3D`, …)
- Same architectural style as 2D
- No forced unification into a single N-D template yet

## Phase 3 — Time-series / 4D
- `ImageSeries2D` / `ImageSeries3D` (or `TemporalImage`)
- Group-wise / longitudinal registration
- Optional 4D motion models

## Phase 4 — Optional Unification (only if clearly beneficial)
- Light shared concepts or templates
- Optional bridges to ITK/OpenCV/VTK (kept outside core)
