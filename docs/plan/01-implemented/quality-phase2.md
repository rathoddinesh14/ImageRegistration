# Implemented: Quality Phase 2

**Status**: done (branch `feature/quality-phase2`)  
**Date**: 2026-09-08

## What was added

1. **`.clang-tidy`** — pragmatic check set (bugprone, analyzer, selected modernize/readability/performance)
2. **CI job `clang-tidy`** — Ubuntu, configure with `CMAKE_EXPORT_COMPILE_COMMANDS`, run on `tests/test_smoke.cpp` (headers filtered via `HeaderFilterRegex`)
3. **`scripts/check_public_api_docs.py`** — fails if non-empty public headers under `include/ir/` declare API without Doxygen-style comments
4. **CI job `public-API docs`** — runs the script on every push/PR

## Notes

- PR template was already delivered with coding standards
- Empty placeholder headers pass the doc check until real declarations appear
- clang-tidy set is intentionally narrow; tighten as the codebase grows
