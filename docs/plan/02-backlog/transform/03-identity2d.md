# Task: Identity2D transform

**Status**: todo  
**Component**: transform  
**Priority**: Low  
**Depends on**: Transform2D (done)

## Goal
Trivial concrete transform: `p' = p`. Useful as default, tests, and composition identity.

## Acceptance criteria
- [ ] `include/ir/transform/Identity2D.hpp` (`final`, implements `Transform2D`)
- [ ] `transformPoint` returns input unchanged
- [ ] `isInvertible()` is `true`
- [ ] Catch2 tests (mapping, polymorphism)
