# ADR 007 — Error handling strategy (hybrid)

**Status**: Proposed  
**Date**: 2026-08-29

## Decision
ImageRegistration adopts a **hybrid** error-handling strategy:

1. **Preconditions / internal bugs**  
   Document preconditions. Check with assertions in debug builds. Violating a precondition is undefined behavior in release unless a specific API documents otherwise.

2. **Recoverable operational failures**  
   Use result types: `ir::Expected<T, E>` and/or structured summaries (e.g. `RegistrationResult`).  
   Typical cases: non-invertible transform, empty overlap, non-finite metric values, validation failures.

3. **Rare or boundary failures**  
   C++ exceptions are allowed when recovery is not a normal local concern (e.g. allocation failure, optional I/O bridges).  
   Public throwing APIs must document `@throws`.

4. **No silent failures**  
   Prefer `[[nodiscard]]` on APIs that return status or expected values.

5. **Ergonomics**  
   Where useful, provide both:
   - `tryX()` → `Expected` / status (noexcept where possible)
   - `x()` → value or throw  

## Rationale
- Registration and optimization **fail often** in real data; exceptions-only is noisy in inner pipelines.
- Eigen/Ceres-style status fits numerical cores; ITK/OpenCV-style exceptions fit some boundaries.
- C++20 has no standard `std::expected` yet; a small `ir::Expected` keeps the door open for C++23.
- Assertions catch misuse early without cluttering release call sites.

## Consequences
- Coding standards section on error handling must be updated when this ADR is **Accepted**.
- Core fallible math APIs should not rely on exceptions as the primary path.
- Contributors follow the decision table in the coding standards / this ADR.
- Implementation work is tracked in backlog tasks `07`–`09` under infrastructure and registration.

## Alternatives considered
- **Exceptions only** — simpler call sites; poor fit for optimizer loops and FFI.
- **Expected only** — consistent but verbose; still need asserts for true bugs.
- **Error codes only (out-params)** — outdated for new C++ public API.

## Status note
This ADR is **Proposed** until the project owner explicitly accepts it. Implementation of `ir::Expected` and error types remains backlog until acceptance.
