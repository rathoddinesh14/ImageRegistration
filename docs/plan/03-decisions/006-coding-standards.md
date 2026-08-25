# ADR 006 — Adopt coding standards document

**Status**: Accepted  
**Date**: 2026-08-26

## Decision
All new and modified code in ImageRegistration must follow the standards described in
`docs/coding-standards.md`.

That document covers:

- Documentation (Doxygen-style for public API)
- Modern C++20 practices (`[[nodiscard]]`, const-correctness, `noexcept`, etc.)
- Naming (aligned with ADR 003)
- Error handling (interim: exceptions + assertions)
- Ownership / lifetime documentation
- Testing and formatting expectations

## Rationale
- Keeps the library readable and consistent as it grows toward 3D and time-series support.
- Makes code review objective (reviewers can point to a single document).
- Complements automated gates (clang-format, warnings-as-errors, tests) with human-facing rules.

## Consequences
- Public headers without parameter/return documentation are considered incomplete.
- PRs should satisfy the checklist in `.github/PULL_REQUEST_TEMPLATE.md`.
- Error-handling strategy may be refined later by a new ADR without rewriting this one.
