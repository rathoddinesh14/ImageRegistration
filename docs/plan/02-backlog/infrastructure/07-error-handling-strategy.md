# Task: Error handling strategy (ADR + standards update)

**Status**: todo  
**Component**: infrastructure  
**Priority**: High (before substantial core API lands)

## Goal
Lock a hybrid error-handling policy for ImageRegistration and reflect it in ADRs and coding standards.

## Proposed policy (hybrid)

| Kind of failure | Mechanism | Examples |
|-----------------|-----------|----------|
| Precondition / internal bug | `assert` (debug); document as precondition | Invalid sizes the API forbids, null misuse |
| Recoverable operational failure | `ir::Expected<T, E>` or result/summary types | Singular transform, empty overlap, non-finite metric |
| Rare / boundary failure | C++ exceptions (documented `@throws`) | OOM, optional I/O bridges later |

Additional rules:
- No silent failures
- `[[nodiscard]]` on functions returning results/expected
- Public fallible APIs prefer `tryX` + optional throwing convenience wrapper where ergonomics matter
- Optimizers / `Registration2D::run` should return a **summary/result** object (success, code, message, stats)

## Deliverables
- [ ] ADR 007 — Error handling strategy (Accepted)
- [ ] Update `docs/coding-standards.md` section 6 (replace interim paragraph)
- [ ] Short decision table for contributors (in ADR or coding standards)
- [ ] Mark this backlog task done

## Out of scope for this task
- Implementing `ir::Expected` itself (separate task)
- Changing production algorithm code (none yet)

## Related
- `08-ir-expected.md` — C++20 expected type
- `09-registration-result-type.md` — result/summary for registration runs
- ADR 006 (coding standards)
