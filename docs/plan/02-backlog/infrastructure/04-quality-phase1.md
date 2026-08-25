# Task: Quality Phase 1 — Foundation (Essential)

**Status**: todo  
**Component**: infrastructure  
**Priority**: High (do early)

## Goal
Establish the minimum quality gate that every change must pass.

## Items in this phase

1. **Catch2 testing infrastructure**  
   (see `02-catch2-testing.md`)

2. **GitHub Actions CI (Windows-first)**  
   (see `01-github-actions-ci.md`)
   - Runner: `windows-latest`
   - Build + run tests
   - Stay within GitHub Free tier

3. **Warnings as errors**
   - MSVC: `/WX`
   - (Future GCC/Clang jobs: `-Werror`)
   - Treat common warning levels as hard failures

4. **Basic clang-format**
   - Add `.clang-format` to the repository
   - CI job (or step) that runs `clang-format --dry-run --Werror` (or equivalent on Windows)
   - Fail the build if formatting differs

## Acceptance criteria
- [ ] Catch2 integrated and a sample test passes on Windows
- [ ] GitHub Actions workflow runs on `windows-latest` and fails on test failure
- [ ] Compiler warnings are treated as errors in CI
- [ ] `.clang-format` exists and format check is part of CI
- [ ] Documented in `01-implemented/` once complete

## Notes
This phase gives immediate protection against broken builds, untested code, and basic style drift. It is the highest-priority quality work.
