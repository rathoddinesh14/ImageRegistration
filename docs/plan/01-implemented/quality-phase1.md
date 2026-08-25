# Implemented: Quality Phase 1 — Foundation

**Status**: done (branch `feature/quality-phase1-finish`)  
**Date**: 2026-08-25

## What was completed

1. **Catch2 testing infrastructure** (earlier)
2. **GitHub Actions CI** on `windows-latest` (earlier)
3. **Warnings as errors**
   - MSVC: `/W4 /WX`
   - GCC/Clang: `-Wall -Wextra -Wpedantic -Werror`
4. **clang-format**
   - `.clang-format` added (LLVM-based, 4-space indent, C++20)
   - CI job `format-check` on `ubuntu-latest` using `jidicula/clang-format-action`

## Notes
- Format check runs on Ubuntu (fast, free-tier friendly for public repos)
- Build + test remains on Windows (primary development platform)
