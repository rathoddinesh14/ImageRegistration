# Task: Write images (PNG via IO module)

**Status**: todo  
**Component**: io (optional; outside core)  
**Priority**: Medium (demos / visual validation)

## Goal
Provide a small, optional **IO** capability to **write** `Image2D` to disk as **PNG**, so examples and experiments can produce visible output without pulling OpenCV into the core library.

Design for **modularity**: v0.1 ships PNG only, but the shape must allow many export formats later without touching core or rewriting a single god-class IO file.

## Design principles
- **Core stays free of I/O** (`Point2D`, `ImageGeometry2D`, `Image2D` unchanged in responsibility).
- IO lives under `include/ir/io/` (and optional compiled helpers under `src/io/` if needed).
- Prefer a **header-only or single-TU** dependency per format (PNG: **stb_image_write**, vendor under `third_party/stb/` or similar).
- Optional in CMake (e.g. `IR_BUILD_IO` and/or enabled when `IR_BUILD_EXAMPLES=ON`).
- Default core target remains **Eigen-only**.

## Modular design approach (locked direction)

### Separation of concerns

| Layer | Responsibility |
|-------|----------------|
| **Core** | `Image2D` pixels + geometry only |
| **Export policy** | How `double` → bytes (scale, clamp, channels) — shared |
| **Format writer** | Encoded bytes → PNG / PGM / BMP / … |

```text
Image2D  →  shared raster policy (e.g. ToRasterU8)  →  PngWriter / PgmWriter / …
```

### Per-format modules (open/closed)

- One format ≈ one writer type + its own header (and TU if needed).
- Adding TIFF later = **new files + CMake wiring**, not edits to `Image2D` or a giant switch in one `ImageIo.cpp`.

Suggested layout:

```text
include/ir/io/
  ExportOptions.hpp     // shared scaling / channel policy
  IImageWriter.hpp      // narrow interface (or C++20 concept)
  PngWriter.hpp         // v0.1
  PgmWriter.hpp         // future
src/io/                 // optional .cpp (e.g. stb in one TU)
  PngWriter.cpp
```

### Writer abstraction

Prefer a **narrow writer interface** (virtual or concept), for example:

```text
// Conceptual — exact names can vary at implementation time
struct ExportOptions { /* scaling mode, bit depth, … */ };

class IImageWriter {
public:
  virtual ~IImageWriter() = default;
  virtual bool write(const Image2D& image, const std::string& path,
                     const ExportOptions& options) const = 0;
};

// Concrete: PngWriter, PgmWriter, …
```

Alternatively a C++20 **concept** + free functions if we want to avoid vtables.

### Optional facade (convenience only)

```text
ir::io::write(image, "out.png");  // dispatch by extension → concrete writer
```

- Thin facade over explicit writers; tests and advanced callers may use `PngWriter` directly.
- v0.1 may only handle `.png`; extend the map as formats appear.
- Prefer a **static extension map** over plugin DLLs for this project.

### Dependencies stay per-format

| Format | Dependency |
|--------|------------|
| PGM/PPM | None |
| PNG | stb_image_write (only linked into PNG writer / IO target) |
| Future codecs | Only that writer's target |

### Anti-patterns (explicitly avoid)

- `Image2D::save(path)` on the core type
- One `writeImage()` with a forever-growing `enum Format` and a huge switch
- OpenCV or libpng as **required** core dependencies
- Re-implementing scaling inside every format writer

### Phased delivery

| Phase | Deliverable |
|-------|-------------|
| **1 (this task)** | `ExportOptions` + grayscale `double→u8` helper + `PngWriter` (stb) + optional path helper for `.png` + one example |
| **2** | `PgmWriter` (zero deps); extension dispatch for `.png` / `.pgm` |
| **3** | Symmetric **read** path; `Expected` for IO errors when that backlog lands |

## Scope (v0.1)

### In scope
- Write **8-bit grayscale PNG** from `ir::Image2D` (`double` pixels).
- Documented **value scaling** policy (choose one and stick to it):
  - **Option A (recommended for synthetic demos):** assume useful range `[0, 1]`, clamp, scale to `0..255`.
  - **Option B:** min–max normalize each image to `0..255`.
  - **Option C:** clamp values already in `[0, 255]`.
- Shared policy helper usable by future writers (even if only PNG exists at first).
- `PngWriter` (and/or `writePng`) under `ir::io`.
- At least one **example** that builds a synthetic image and writes `*.png`.
- Failure reporting: `bool` return for v0.1; migrate to `ir::Expected` when that backlog lands.

### Out of scope (later)
- Read / decode PNG (or JPEG)
- Multi-channel RGB/RGBA in core
- OpenCV / libpng as required dependencies
- Streaming or in-memory encode without a file path
- Windows GUI preview windows
- Full plugin/DLL format system

## Acceptance criteria
- [ ] Public header(s) under `include/ir/io/` with Doxygen-style docs (`/** */`)
- [ ] Modular shape documented in code layout: shared export policy + `PngWriter` (not core `save`)
- [ ] stb (or equivalent) vendored/fetched without polluting core target for default builds
- [ ] `PngWriter` / `writePng` succeeds for a small synthetic `Image2D` in an example or test
- [ ] Scaling policy documented in the header and/or `docs/`
- [ ] CMake option keeps default core build Eigen-only
- [ ] CI remains green (Windows); format/tidy as applicable
- [ ] Plan: mark this task done + short note under `docs/plan/01-implemented/` when complete

## Suggested API sketch (v0.1)

```text
namespace ir::io {
  struct ExportOptions { /* scaling mode */ };

  class PngWriter {
  public:
    bool write(const Image2D& image, const std::string& path,
               const ExportOptions& options = {}) const;
  };

  // Optional convenience
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
