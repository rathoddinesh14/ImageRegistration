# Task: Quality Phase 2 — Static Analysis & Documentation Checks

**Status**: todo  
**Component**: infrastructure  
**Priority**: Medium (after Phase 1 is green)

## Goal
Catch deeper code-quality and documentation problems automatically.

## Items in this phase

1. **clang-tidy**
   - Add `.clang-tidy` configuration
   - Run clang-tidy in CI (Windows and/or Ubuntu)
   - Enable useful checks for modern C++, const-correctness, readability, bugprone patterns
   - Start with a pragmatic set; tighten over time

2. **Documentation presence check**
   - Lightweight script or tool that verifies public headers contain documentation comments for:
     - Classes
     - Public methods (parameters + return)
   - Fail CI (or warn) when public API lacks docs
   - Can be a simple Python/PowerShell script or Doxygen-based check

3. **PR template / checklist** — **done** (delivered with coding-standards task)
   - `.github/PULL_REQUEST_TEMPLATE.md` exists with quality checklist

## Acceptance criteria
- [ ] `.clang-tidy` present and running in CI
- [ ] Basic public-API documentation check exists
- [x] PR template with quality checklist
- [x] Phase 1 still fully green

## Notes
Phase 2 builds on the solid foundation of Phase 1. It starts enforcing the documentation and modern-C++ rules defined in the coding-standards task.
