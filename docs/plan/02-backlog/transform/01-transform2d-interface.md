# Task: Transform2D (pure interface)

**Status**: in progress (TDD — tests first on branch `feature/transform2d-interface-tdd`)  
**Component**: transform

## Goal
Define the pure abstract interface for 2D spatial transforms.

## Locked design (driven by tests)
- Pure abstract class `ir::Transform2D` (not a concept in v0.1)
- `Point2D transformPoint(const Point2D&) const` — pure virtual
- `bool isInvertible() const noexcept` — pure virtual; reports capability
- Virtual destructor for polymorphic ownership (`unique_ptr<Transform2D>`)
- **No** inverse object API in v0.1 (deferred until `ir::Expected` / Error types)
- **No** production concrete transforms in this task (test stubs only)

## Acceptance criteria
- [ ] Header: `include/ir/transform/Transform2D.hpp`
- [x] Unit tests: `tests/transform/test_transform2d.cpp` (written first)
- [ ] Documented ownership and lifetime expectations
- [ ] No concrete production implementations in this task

## Notes
This is the key abstraction that metrics, resamplers, and the registration orchestrator will depend on.
