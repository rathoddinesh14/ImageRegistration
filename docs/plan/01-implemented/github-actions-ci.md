# Implemented: GitHub Actions CI

**Status**: done (branch `feature/github-actions-ci`)  
**Date**: 2026-08-25

## What was added
- `.github/workflows/ci.yml`
- Single job on `windows-latest`
- Configures with `IR_BUILD_TESTS=ON`, builds Release, runs CTest
- Eigen is fetched via FetchContent when not found on the system (CI-friendly)

## Triggers
- Push to `main`
- Pull requests targeting `main`

## Notes
- Kept minimal for GitHub Free tier
- Primary platform matches the developer environment (Windows)
- Future: Ubuntu job, clang-format, clang-tidy (see quality phases)
