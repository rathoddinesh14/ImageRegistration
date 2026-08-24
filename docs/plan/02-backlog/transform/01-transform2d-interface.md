# Task: Transform2D (pure interface)

**Status**: todo  
**Component**: transform

## Goal
Define the pure abstract interface for 2D spatial transforms.

## Acceptance criteria
- Header: `include/ir/transform/Transform2D.hpp`
- Pure abstract class (or C++20 concept)
- At minimum:
  - `Point2D transformPoint(const Point2D&) const = 0;`
  - Ability to obtain the inverse (or a clear statement that not all transforms are invertible)
- No concrete implementations in this task
- Documented ownership and lifetime expectations

## Notes
This is the key abstraction that metrics, resamplers, and the registration orchestrator will depend on.
