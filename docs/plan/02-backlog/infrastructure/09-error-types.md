# Task: Error codes and Error type

**Status**: todo  
**Component**: core / infrastructure  
**Priority**: Medium (with or right after `ir::Expected`)

## Goal
Define a small, stable set of error codes and an `Error` type for recoverable failures.

## Draft codes (to refine in implementation)

```text
Ok
SingularTransform
EmptyOverlap
NonFiniteMetric
InvalidGeometry
SizeMismatch
NotImplemented
Unknown
```

## Requirements
- `enum class ErrorCode`
- `class Error` (or equivalent) holding code + optional message
- Documented public headers
- Used as `E` in `ir::Expected<T, Error>` (or `Expected<T, ErrorCode>` if we keep it minimal)
- Tests for construction and comparison

## Notes
Keep the set small at first; extend as real APIs appear (registration, metrics, IO bridges).

## Related
- `07-error-handling-strategy.md`
- `08-ir-expected.md`
