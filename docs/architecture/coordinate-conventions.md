# Coordinate and indexing conventions

**Status:** authoritative for ImageRegistration (2D now; extend the same rules to 3D and time-series later)  
**Related code:** `ImageGeometry2D`, `Image2D`, `Point2D`, `Transform2D`

This document defines how index space, physical space, and pixel samples relate. Implementations must follow these rules so registration, resampling, and metrics stay consistent.

---

## 1. Two spaces

| Space | Meaning | Typical type |
|-------|---------|----------------|
| **Index space** | Discrete (or continuous) lattice coordinates on the image grid | integers `(i, j)` or continuous indices |
| **Physical space** | Continuous world / patient / lab coordinates | `Point2D` (`double`) |

Transforms (`Transform2D`) act on **physical** points unless a component explicitly documents otherwise.

---

## 2. Index axes and bounds

For a 2D image of size `width × height`:

- **`i`** — index along width, valid integers in `[0, width)`
- **`j`** — index along height, valid integers in `[0, height)`

Empty images are not allowed: `width >= 1` and `height >= 1`.

### Storage layout (`Image2D`)

Pixels are stored in a contiguous buffer in **row-major** order:

```text
linearIndex = j * width + i
```

So `i` changes fastest when walking memory.

---

## 3. Pixel-center convention

**Origin** is the physical position of the **center** of pixel `(0, 0)`, not a corner of the pixel.

Consequences:

- Integer index `(i, j)` refers to the **center** of that pixel.
- Continuous indices (e.g. from `physicalToIndex`) may be fractional; they still live in the same continuous lattice where integers mark pixel centers.
- Half-pixel offsets appear when mapping to corner-based systems (e.g. some graphics APIs).

```text
Pixel (0,0) extent in index space (conceptual):
  corners near (-0.5, -0.5) … (0.5, 0.5)
  center at (0, 0)  ← what origin maps to in physical space for (0,0)
```

---

## 4. Spacing

**Spacing** is the physical length of **one index step** along each local axis:

- `spacing.x()` — physical size of one step in `i`
- `spacing.y()` — physical size of one step in `j`

Units are application-defined (mm, meters, pixels-as-unit, etc.). The library does not attach unit tags; callers must keep units consistent across fixed and moving images.

---

## 5. Direction matrix

**Direction** is a `2×2` matrix whose **columns** are the local image axes expressed in physical space.

- Column 0 — direction of increasing `i`
- Column 1 — direction of increasing `j`

**Identity** means axis-aligned: `i` increases along physical `+x`, `j` along physical `+y`.

Direction may rotate (and, if ever needed, reflect) the lattice relative to physical axes. Spacing scales the step **before** applying direction (see mapping below).

---

## 6. Index ↔ physical mapping

For geometry with origin `o`, spacing `s`, direction `D`:

**Index → physical** (integer or continuous index components `i`, `j`):

```text
local = (i * s.x, j * s.y)
physical = o + D * local
```

**Physical → index** (continuous):

```text
local = D^{-1} * (physical - o)
i = local.x / s.x
j = local.y / s.y
```

Implemented by `ImageGeometry2D::indexToPhysical` and `physicalToIndex`.

Round-trip holds for non-singular `D` and non-zero spacing (within floating-point tolerance).

---

## 7. Transforms and registration

- `Transform2D::transformPoint` maps **physical → physical**.
- Resampling a moving image into fixed space typically:
  1. Take a fixed-image index (or physical point)
  2. Map to physical (fixed geometry)
  3. Apply the inverse moving←fixed transform (when available)
  4. Map to moving continuous index (moving geometry)
  5. Interpolate moving samples

Exact pipeline types will land in interpolator / resampler modules; they must obey these geometry rules.

---

## 8. Extension to 3D and time

When 3D or time-series types appear, reuse the same ideas:

| 2D | 3D / time |
|----|-----------|
| `(i, j)` | `(i, j, k)` and optionally time index `t` |
| Pixel center | Voxel center (and sample-center in time if discrete) |
| Spacing `Point2D` | Spacing vector with one entry per axis |
| Direction `2×2` | Direction `3×3` (spatial); time usually separate |

Do not switch to corner-based origins in 3D without an explicit, documented ADR.

---

## 9. Summary table

| Concept | Rule |
|---------|------|
| Index `(i, j)` | `i ∈ [0, width)`, `j ∈ [0, height)` |
| Origin | Physical position of **center** of `(0, 0)` |
| Spacing | Physical size of one step in `i` / `j` |
| Direction | Columns = local axes in physical space |
| Mapping | `physical = origin + direction * (i·sx, j·sy)` |
| Buffer layout | Row-major: `j * width + i` |
| Transforms | Physical space unless documented otherwise |

---

## References in code

- `include/ir/core/ImageGeometry2D.hpp`
- `include/ir/core/Image2D.hpp`
- `include/ir/core/Point2D.hpp`
- `include/ir/transform/Transform2D.hpp`
