# Task: RegistrationResult / solve summary type

**Status**: todo  
**Component**: registration  
**Priority**: Medium (before implementing Registration2D::run)

## Goal
Define a result/summary type for registration runs so success/failure and diagnostics are explicit (Ceres-style), not only exceptions or a bare bool.

## Draft shape

```cpp
struct RegistrationResult {
    bool succeeded = false;
    ErrorCode code = ErrorCode::Ok;
    // transform parameters or Transform2D — exact type TBD
    double finalMetric = 0.0;
    int iterations = 0;
    // optional: message string_view or Error
};
```

## Requirements
- Header under `include/ir/registration/`
- Documented fields and validity rules (e.g. transform valid only if `succeeded`)
- Used as return type of `Registration2D::run` (future)
- Unit tests for default state and typical success/failure values
- Aligns with hybrid error-handling ADR

## Related
- `docs/plan/02-backlog/infrastructure/07-error-handling-strategy.md`
- Future `Registration2D` interface task
