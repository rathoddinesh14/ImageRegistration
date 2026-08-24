# Implemented: CMake & Dependencies

**Status**: done (basic scaffolding)

- CMake ≥ 3.16, C++20 required
- Main interface library target: `ImageRegistration` (alias `ir::ImageRegistration`)
- Public include directory: `include/`
- Required dependency: Eigen3
- Optional flags:
  - `IR_BUILD_TESTS`
  - `IR_BUILD_EXAMPLES`
  - `IR_BUILD_BENCHMARKS`
- Common compiler warnings enabled

**Next**: integrate Catch2 when the first tests are written.
