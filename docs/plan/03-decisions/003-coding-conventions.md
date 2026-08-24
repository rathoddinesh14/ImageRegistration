# ADR 003 — Coding conventions

**Status**: Accepted  
**Date**: 2026-08-25

## Decisions

| Item                    | Convention              |
|-------------------------|-------------------------|
| Header guard            | `#pragma once`          |
| Classes / structs       | `PascalCase`            |
| Methods / free functions| `camelCase`             |
| Member variables        | `m_` prefix             |
| Namespaces              | `ir` (and sub-namespaces later if needed) |

## Example

```cpp
#pragma once

namespace ir {

class Image2D {
public:
    int width() const;
    void setWidth(int w);

private:
    int m_width = 0;
};

} // namespace ir
```
