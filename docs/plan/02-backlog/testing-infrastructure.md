# Task: Testing infrastructure (Catch2)

**Status**: todo  
**Component**: infrastructure

## Goal
Integrate Catch2 so that unit tests can be written and run.

## Acceptance criteria
- When `IR_BUILD_TESTS=ON`, Catch2 is fetched via CMake `FetchContent` (or similar)
- A simple “hello test” compiles and runs
- `ctest` (or the Catch2 runner) discovers the tests
- Documentation updated in README / plan

## Notes
Do this early so every subsequent data-structure task can include tests from day one.
