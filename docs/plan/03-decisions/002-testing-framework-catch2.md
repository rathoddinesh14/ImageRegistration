# ADR 002 — Testing framework: Catch2

**Status**: Accepted  
**Date**: 2026-08-25

## Decision
Use Catch2 as the unit-testing framework.

## Rationale
- Modern, expressive syntax
- Header-only option available
- Excellent CMake support via FetchContent
- Widely adopted in contemporary C++ projects

## Consequences
- Tests will live under `tests/`
- CMake will fetch Catch2 when `IR_BUILD_TESTS=ON`
