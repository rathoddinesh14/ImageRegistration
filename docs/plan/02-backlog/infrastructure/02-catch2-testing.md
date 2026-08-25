# Task: Testing infrastructure (Catch2)

**Status**: done (on branch `feature/catch2-testing`)  
**Component**: infrastructure  
**Priority**: High

## Goal
Integrate Catch2 so that unit tests can be written and run.

## Acceptance criteria
- [x] When `IR_BUILD_TESTS=ON`, Catch2 is fetched via CMake `FetchContent`
- [x] A simple “hello test” compiles and runs
- [x] `ctest` / Catch2 discovers the tests (`catch_discover_tests`)
- [x] Works on Windows (no Unix-specific assumptions)
- [ ] Documentation updated in README / plan (pending final merge)

## Implementation notes
- Catch2 v3.5.2 fetched with `FetchContent`
- Test executable: `ir_tests`
- Smoke test: `tests/test_smoke.cpp`
- Linked against `ir::ImageRegistration` and `Catch2::Catch2WithMain`

## Notes
- Do this early so every subsequent data-structure task can include tests from day one.
- This task and the GitHub Actions CI task (`01-github-actions-ci.md`) should be done together or in quick succession so that CI has real tests to execute.
