# Implemented: Error handling strategy (ADR 007)

**Status**: done (branch `feature/accept-adr-007-error-handling`)  
**Date**: 2026-09-16

## What was done
- ADR 007 status set to **Accepted**
- Decision table added to the ADR
- `docs/coding-standards.md` §6 rewritten from interim exceptions text to hybrid policy
- Backlog task `07-error-handling-strategy` marked done

## Policy summary
Asserts for preconditions → `Expected`/summaries for recoverable failures → exceptions for rare/boundary cases only.

## Follow-ups
- `08-ir-expected.md`, `09-error-types.md`, `registration/01-registration-result.md`
