# Task: Write images (PNG via IO module)

**Status**: in progress (TDD — tests first on branch `feature/io-write-png-tdd`)  
**Component**: io (optional; outside core)  
**Priority**: Medium (demos / visual validation)

## Goal
Provide a small, optional **IO** capability to **write** `Image2D` to disk as **PNG**, so examples and experiments can produce visible output without pulling OpenCV into the core library.

## Modular design (locked direction)
- Core stays free of I/O.
- Shared **export policy** (`double` → bytes) + per-format **writers** (`PngWriter`, future `PgmWriter`).
- Optional path facade later; v0.1 focuses on `PngWriter` / `writePng`.
- stb_image_write only for PNG target, not core.

## Scaling (v0.1 locked for tests)
- **Clamp01**: treat pixel values as roughly `[0, 1]`, clamp to `[0, 1]`, scale to `0..255` byte.

## Acceptance criteria
- [ ] `include/ir/io/` headers + PNG writer implementation
- [x] Unit tests: `tests/io/test_png_writer.cpp` (written first)
- [ ] Example or test produces a real `.png` on disk
- [ ] Default core build remains Eigen-only
