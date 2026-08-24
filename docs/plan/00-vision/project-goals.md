# Project Goals

- Build a clean, modular, extensible **2D** image-registration library in modern C++ (C++20).
- Design the architecture so that **3D** and **time-series (2D+t / 3D+t)** support can be added later without major rewrites.
- Prefer composition over deep inheritance.
- Keep the public API under `include/ir/`.
- Keep the core library independent of VTK, ITK, OpenCV, Qt, and DICOM libraries.
- Use Eigen for linear algebra.
- Provide a solid foundation of data structures and pure interfaces before implementing algorithms.
