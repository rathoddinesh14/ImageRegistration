# Task: Write images (PNG via IO module)

**Status**: todo  
**Component**: io (optional; outside core)  
**Priority**: Medium (demos / visual validation)

## Goal
Provide a small, optional **IO** capability to **write** `Image2D` to disk as **PNG**, so examples and experiments can produce visible output without pulling OpenCV into the core library.

## Design principles
- **Core stays free of I/O** (`Point2D`, `ImageGeometry2D`, `Image2D` unchanged in responsibility).
- IO lives under `include/ir/io/` (and optional compiled helpers if needed).
- Prefer a **header-only or single-TU** dependency: **stb_image_write** (vendor under `third_party/stb/` or similar).
- Optional in CMake (e.g. `IR_BUILD_IO` and/or enabled when `IR_BUILD_EXAMPLES=ON`).

## Scope (v0.1)

### In scope
- Write **8-bit grayscale PNG** from `ir::Image2D` (`double` pixels).
- Documented **value scaling** policy (choose one and stick to it):
  - **Option A (recommended for synthetic demos):** assume useful range `[0, 1]`, clamp, scale to `0..255`.
  - **Option B:** min–max normalize each image to `0..255`.
  - **Option C:** clamp values already in `[0, 255]`.
- API under `ir::io` (e.g. `writePng(const Image2D&, path)`).
- At least one **example** that builds a synthetic image and writes `*.png`.
- Failure reporting: `bool` return or message for v0.1; migrate to `ir::Expected` when that backlog lands.

### Out of scope (later backlog)
- Read / decode PNG (or JPEG)
- Multi-channel RGB/RGBA images in core
- OpenCV / libpng as required dependencies
- Streaming or in-memory encode without a file path
- Windows GUI preview windows

## Acceptance criteria
- [ ] Public header(s) under `include/ir/io/` with Doxygen-style docs (`/** */`)
- [ ] stb (or equivalent) vendored/fetched without polluting core target link line for default builds
- [ ] `writePng` (or equivalent) succeeds for a small synthetic `Image2D` in an example or test
- [ ] Scaling policy documented in the header and/or `docs/`
- [ ] CMake option keeps default core build Eigen-only
- [ ] CI remains green (Windows); format/tidy as applicable
- [ ] Plan: mark this task done + short note under `docs/plan/01-implemented/` when complete

## Suggested API sketch

```text
namespace ir::io {
  bool writePng(const Image2D& image, const std::string& path);
}
```

## Implementation notes
- Row-major layout of `Image2D` matches typical image writers (`y * width + x`).
- Geometry (spacing/origin) is **not** embedded in PNG unless we add metadata later; PNG is display/debug only for v0.1.
- Prefer ASCII-only paths in tests/examples for CI simplicity.

## Related
- `docs/plan/02-backlog/core/03-image2d.md` (done)
- Future: read image, RGB, `Expected` for IO errors
- CMake folder layout snag list (`infrastructure/10-cmake-folder-layout.md`) when examples/IO targets grow

## Not this task
- Registration algorithms, transforms, metrics
