# ADR 004 — Pure abstract interfaces first

**Status**: Accepted  
**Date**: 2026-08-25

## Decision
All major abstractions (`Transform2D`, `Interpolator2D`, `Metric2D`, `Optimizer`, `Resampler2D`, `Registration2D`, …) start as pure abstract interfaces (or concepts).  
Concrete implementations are added only after the interfaces are stable.

## Rationale
- Forces clear contracts
- Makes dependency inversion natural
- Allows multiple implementations to coexist
- Matches the “data structures & interfaces first” strategy

## Consequences
- Early headers contain only pure virtual methods (or C++20 concepts)
- No algorithm logic in the first interface commits
