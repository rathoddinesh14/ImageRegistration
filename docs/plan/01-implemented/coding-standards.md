# Implemented: Coding Standards

**Status**: done (branch `feature/coding-standards`)  
**Date**: 2026-08-26

## What was added
- `docs/coding-standards.md` — documentation, modern C++, ownership, testing, formatting rules
- `docs/plan/03-decisions/006-coding-standards.md` — ADR adopting the standards
- `.github/PULL_REQUEST_TEMPLATE.md` — PR checklist aligned with the standards

## Notes
- Naming remains as in ADR 003 (`PascalCase` types, `camelCase` methods, `m_` members)
- Public API documentation is mandatory (Doxygen-style)
- Error handling is interim (exceptions + assertions); may be refined by a later ADR
