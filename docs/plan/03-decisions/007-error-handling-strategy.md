# ADR 007 — Error handling strategy (hybrid)

**Status**: Accepted  
**Date**: 2026-08-29 (accepted 2026-09-16)

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

## Decision table (for contributors)

| Question | Mechanism |
|----------|-----------|
| Broken contract if the program is correct? | Assert (debug) + document precondition |
| Can valid callers hit this with hard real data? | `ir::Expected` or result/summary type |
| Extremely rare or only at app/IO boundary? | Exception + `@throws` |
| Need success flag + diagnostics together (e.g. optimize)? | Summary struct (e.g. `RegistrationResult`) |

## Rationale
- Registration and optimization **fail often** in real data; exceptions-only is noisy in inner pipelines.
- Eigen/Ceres-style status fits numerical cores; ITK/OpenCV-style exceptions fit some boundaries.
- C++20 has no standard `std::expected` yet; a small `ir::Expected` keeps the door open for C++23.
- Assertions catch misuse early without cluttering release call sites.

## Consequences
- `docs/coding-standards.md` §6 follows this ADR (no longer interim).
- Core fallible math APIs should not rely on exceptions as the primary path.
- Contributors follow the decision table above.
- Implementation of `Error` / `ir::Expected` / `RegistrationResult` remains backlog tasks `08`, `09`, and registration result.

## Alternatives considered
- **Exceptions only** — simpler call sites; poor fit for optimizer loops and FFI.
- **Expected only** — consistent but verbose; still need asserts for true bugs.
- **Error codes only (out-params)** — outdated for new C++ public API.
