# Implemented: PNG write (ir::io)

**Status**: done (branch `feature/io-write-png-tdd`)  
**Date**: 2026-10-02

## What was added
- `ir::io::ExportOptions`, `ScalingMode::Clamp01`
- `ir::io::PngWriter` / `writePng` (stb_image_write)
- CMake target `ir_io` / `ir::io`
- Tests + sample PNGs: gradient, disk, checkerboard

## View samples
`artifacts/io_png_samples/sample_*.png` or set `IR_PNG_SAMPLE_DIR` when running tests.
