# Design Principles

1. **Data structures first, algorithms second**  
   Solid types and pure interfaces come before any registration logic.

2. **Do not reinvent the wheel**  
   Reuse Eigen, Catch2, and other mature libraries for math, testing, etc.

3. **Separation of concerns**  
   Core geometry & images must not depend on I/O or visualization.

4. **Dependency inversion**  
   High-level registration depends on abstractions (Transform, Metric, Interpolator, Optimizer), not concrete implementations.

5. **Composition over inheritance**  
   Prefer composing small, focused components.

6. **Minimal core dependencies**  
   Only Eigen is required for the core library. Everything else is optional.

7. **Incremental evolution**  
   2D first → parallel 3D modules → time-series layer.
