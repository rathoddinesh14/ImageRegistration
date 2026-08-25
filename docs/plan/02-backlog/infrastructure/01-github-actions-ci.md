# Task: GitHub Actions CI

**Status**: done (branch `feature/github-actions-ci`)  
**Component**: infrastructure  
**Priority**: High

## Goal
Set up continuous integration using GitHub Actions that builds the library and runs tests on every push / pull request.

## Constraints (from project owner)
- Primary target platform: **Windows** (developer only has a Windows machine)
- Must stay within **GitHub Free** tier limits (public repository)
- No paid GitHub features required

## Implementation (completed)
- Workflow: `.github/workflows/ci.yml`
- Runner: `windows-latest`
- Eigen fetched automatically via FetchContent when not present
- Build + CTest with `IR_BUILD_TESTS=ON`

## Acceptance criteria
- [x] `.github/workflows/ci.yml` exists and is valid
- [x] Workflow targets `windows-latest`
- [x] Library builds with `IR_BUILD_TESTS=ON`
- [x] Tests are executed and the job fails if tests fail
- [ ] Status badge can be added to README later
- [x] Documented in the plan (`01-implemented/`)

## Related tasks
- Catch2 testing infrastructure (done)
- Future: Ubuntu job, clang-format check, sanitizers, coverage
