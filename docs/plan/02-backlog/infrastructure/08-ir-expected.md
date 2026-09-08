# Task: `ir::Expected<T, E>` (C++20)

**Status**: todo  
**Component**: core / infrastructure  
**Priority**: Medium (after error-handling ADR is accepted)

## Goal
Provide a small, header-only result type for recoverable failures until the project standardizes on C++23 `std::expected`.

## Requirements
- Header under `include/ir/` (e.g. `include/ir/core/Expected.hpp` or `include/ir/Expected.hpp`)
- Value semantics; no exceptions required to use it
- API roughly aligned with `std::expected`:
  - `has_value()`, `value()`, `error()`
  - `operator bool`
  - construction from value or error
- `E` defaulting to a project `Error` / `ErrorCode` type (defined in the same effort or adjacent)
- Full Doxygen-style docs per coding standards
- Catch2 tests (success path, error path, nodiscard behavior if applicable)
- ASCII-only test names

## Design constraints
- Header-only, no extra third-party dependency
- Fits hybrid policy: use for singular transforms, validation, non-finite metrics, etc.
- Does not replace asserts for precondition violations

## Acceptance criteria
- [ ] Header compiles on MSVC (Windows CI)
- [ ] Unit tests pass
- [ ] Documented and referenced from coding standards / ADR 007
- [ ] clang-format clean

## Related
- `07-error-handling-strategy.md`
- Future `Transform2D::tryInverse()` style APIs
