# Task: Write images (PNG via IO module)

**Status**: done (branch `feature/io-write-png-tdd`)  
**Component**: io (optional; outside core)  
**Priority**: Medium (demos / visual validation)

## Goal
Optional **IO** to write `Image2D` as **PNG** (stb_image_write), core stays free of I/O.

## Modular design
- Shared `ExportOptions` / `ScalingMode::Clamp01`
- Per-format `PngWriter` (future writers can share raster policy)
- CMake target `ir::io` (`IR_BUILD_IO`; auto-on with tests)

## Acceptance criteria
- [x] `include/ir/io/ExportOptions.hpp`, `PngWriter.hpp` + `src/io/PngWriter.cpp`
- [x] Unit tests `tests/io/test_png_writer.cpp` (geometry + sample PNG artifacts)
- [x] Sample PNGs under `artifacts/io_png_samples/`
- [x] Core remains Eigen-only by default
