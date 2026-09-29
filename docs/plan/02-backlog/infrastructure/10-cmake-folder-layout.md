# Task: CMake folder-level layout (snag list)

**Status**: todo  
**Component**: infrastructure  
**Priority**: Low (track now; implement when `src/` or examples grow)

## Goal
Keep a clear, scalable CMake layout: **one root project**, folder-level `CMakeLists.txt` only where we define **targets** (tests, examples, benchmarks, future compiled libs).

## Current state (after Point2D)

| Path | Role |
|------|------|
| `CMakeLists.txt` (root) | `project()`, C++20, warnings, Eigen, INTERFACE library, options |
| `tests/CMakeLists.txt` | `ir_tests` executable + Catch discovery |
| `examples/CMakeLists.txt` | Placeholder only |
| `benchmarks/CMakeLists.txt` | Placeholder only |
| `cmake/` | Empty placeholder (`.gitkeep`) |

Root is the only entry point. Subdirs are pulled in via `add_subdirectory` when options are ON.

## Snag list (future work)

Track these so we do not invent ad-hoc CMake later:

1. **Do not add** a `CMakeLists.txt` per tiny header folder under `include/ir/` (headers stay INTERFACE from root).
2. **When `.cpp` appears**, introduce `src/CMakeLists.txt` (or `src/<module>/`) for compiled targets; keep public headers under `include/`.
3. **Examples**: flesh out `examples/CMakeLists.txt` with one or more executables linked to `ir::ImageRegistration` when demos exist.
4. **Benchmarks**: same for `benchmarks/` (optional Google Benchmark or simple timers later).
5. **`cmake/` helpers**: move repeated FetchContent / warning / install logic into `cmake/*.cmake` and `include()` from root when duplication hurts.
6. **Install / export**: `install(TARGETS ...)`, `install(DIRECTORY include/)`, package config — only when consumers need find_package.
7. **Presets** (optional): `CMakePresets.json` for Windows MSVC + `IR_BUILD_TESTS=ON` to match CI and local VS workflows.
8. **Avoid** nested `project()` calls in subdirectories.
9. **Eigen FetchContent**: watch Fortran/third-party probe issues on odd PATH setups; keep MSVC `/external:*` (already on main).

## Acceptance criteria (when this task is picked up)

- [ ] Documented layout in this file still matches the repo (or file is updated)
- [ ] Any new folder-level CMake follows “target per directory,” not “file per directory”
- [ ] Root remains the only `project()` and the only place that sets global flags/options
- [ ] Optional components stay behind `IR_BUILD_*` options

## Out of scope

- Changing the Point2D or test logic
- Adding new library features

## Related

- Root `CMakeLists.txt`
- `tests/CMakeLists.txt`
- Vision / design principles under `docs/plan/00-vision/`
