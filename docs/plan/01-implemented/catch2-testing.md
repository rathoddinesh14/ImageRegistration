# Implemented: Catch2 Testing Infrastructure

**Status**: done (branch `feature/catch2-testing`)  
**Date**: 2026-08-25

## What was added
- Catch2 v3.5.2 integrated via CMake `FetchContent` (only when `IR_BUILD_TESTS=ON`)
- Test executable `ir_tests`
- Smoke test `tests/test_smoke.cpp`
- Tests discovered automatically with `catch_discover_tests`

## How to run locally
```bash
cmake -S . -B build -DIR_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Notes
- Primary target platform: Windows
- No production code changed
