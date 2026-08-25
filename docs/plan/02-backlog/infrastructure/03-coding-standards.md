# Task: Coding Standards, Documentation & Modern C++ Practices

**Status**: todo  
**Component**: infrastructure  
**Priority**: High

## Goal
Define the coding standards, documentation conventions, and modern C++ practices that all source code must follow. These standards are enforced progressively through the three quality phases below.

## Scope

### Documentation
- Public headers use Doxygen-style comments (`///` or `/** */`)
- Every public function/method documents:
  - Purpose
  - Parameters (meaning, units, constraints)
  - Return value
  - Pre-/post-conditions and error behavior where relevant
- Class-level documentation of responsibility and ownership

### Modern C++ (C++20)
- `[[nodiscard]]` where appropriate
- Strong const-correctness
- `noexcept` where correct
- `enum class`, `override`/`final`
- Prefer value semantics / smart pointers over raw owning pointers
- Clear, small functions; no magic numbers

### Production habits
- Explicit error-handling strategy (to be decided: exceptions vs `std::expected`/error codes)
- Assertions for internal invariants (debug builds)
- Clear ownership and lifetime rules at interfaces

## Deliverables
- Coding standards document (under `docs/` or `docs/plan/`)
- Supporting ADR
- Short examples of well-documented headers
- PR checklist items

## Related
- Phase 1–3 quality enforcement tasks
- Existing ADR 003 (naming conventions)
