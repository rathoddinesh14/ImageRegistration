# Task: Translation2D

**Status**: done (branch `feature/translation2d`)  
**Component**: transform

## Goal
Concrete 2D translation implementing `Transform2D`.

## Design
- `p' = p + offset`
- Always invertible
- Header-only value type (`final`)

## Acceptance criteria
- [x] `include/ir/transform/Translation2D.hpp`
- [x] Tests: `tests/transform/test_translation2d.cpp`
