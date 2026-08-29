# Coding Standards — ImageRegistration

This document defines how we write and document C++ code in this library.
It complements [ADR 003 (naming conventions)](plan/03-decisions/003-coding-conventions.md)
and [ADR 006 (coding standards adoption)](plan/03-decisions/006-coding-standards.md).

## 1. General principles

- Prefer clarity over cleverness.
- Keep public headers minimal and self-contained.
- Data structures and pure interfaces first; algorithms second.
- Core library stays free of I/O, visualization, and heavy frameworks.

## 2. Naming (see also ADR 003)

| Item | Convention |
|------|------------|
| Types (classes, structs, enums) | `PascalCase` |
| Functions / methods | `camelCase` |
| Member variables | `m_` prefix |
| Constants / enum enumerators | `PascalCase` or `kCamelCase` for constexpr values |
| Namespaces | `ir` (sub-namespaces only when clearly needed) |
| Files | Match the primary type: `Point2D.hpp`, `test_point2d.cpp` |

## 3. Headers and includes

- Use `#pragma once`.
- Order includes: related header → project headers → third-party → standard library.
- Prefer forward declarations in headers when sufficient.
- Do not put implementation details in public headers unless required for templates/inline.

## 4. Documentation (Doxygen-style)

Public API **must** be documented.

Use `///` (or `/** */`) on:

- Every public class / struct (responsibility, ownership)
- Every public function / method:
  - Brief purpose
  - `@param` for each parameter (meaning, units, constraints)
  - `@return` when not `void`
  - Pre/post-conditions or error behavior when relevant

### Example

```cpp
#pragma once

namespace ir {

/// A 2D point in continuous coordinates.
///
/// Value type with no dynamic allocation. Suitable for geometry and sampling.
class Point2D {
public:
    /// Construct a point from Cartesian coordinates.
    /// @param x Horizontal coordinate.
    /// @param y Vertical coordinate.
    Point2D(double x, double y);

    /// @return The x-coordinate.
    [[nodiscard]] double x() const noexcept;

    /// @return The y-coordinate.
    [[nodiscard]] double y() const noexcept;

private:
    double m_x = 0.0;
    double m_y = 0.0;
};

} // namespace ir
```

## 5. Modern C++ (C++20)

- Mark functions `[[nodiscard]]` when ignoring the result is likely a bug.
- Prefer strong **const-correctness** (`const` methods, const references).
- Use `noexcept` where the function genuinely does not throw.
- Prefer `enum class` over plain enums.
- Use `override` / `final` explicitly on virtual methods.
- Prefer value semantics; use `std::unique_ptr` for exclusive ownership; avoid raw owning pointers.
- Prefer `nullptr` over `NULL` or `0`.
- Avoid magic numbers; use named constants.
- Keep functions small and focused.

## 6. Error handling (interim)

Until a dedicated ADR decides otherwise:

- Prefer **exceptions** for genuine error conditions in library code that cannot be handled locally.
- Use assertions (`assert` / similar) for internal programming errors in debug builds.
- Document what public functions may throw.
- Do not use exceptions for normal control flow.

(A future ADR may adopt `std::expected` or error codes for specific subsystems.)

## 7. Ownership and lifetime

- Document who owns memory and how long references/pointers remain valid.
- Prefer returning by value for small types (`Point2D`, etc.).
- For non-owning views, document lifetime requirements clearly at the interface.

## 8. Testing

- Every new public type or non-trivial function should have Catch2 tests.
- Test names use ASCII only (no Unicode punctuation) so CTest filters work on Windows.
- Prefer focused unit tests over large integration tests early on.

## 9. Formatting

- All code must match `.clang-format`.
- CI enforces formatting; format before pushing (`clang-format -i <files>`).

## 10. Pull request expectations

See `.github/PULL_REQUEST_TEMPLATE.md`. At minimum:

- [ ] Public API documented (params + return)
- [ ] Tests added or updated
- [ ] Follows these coding standards
- [ ] CI is green
