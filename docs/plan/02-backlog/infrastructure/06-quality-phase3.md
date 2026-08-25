# Task: Quality Phase 3 — Deeper Assurance (Later)

**Status**: todo  
**Component**: infrastructure  
**Priority**: Low / later

## Goal
Add stronger correctness and health signals once the library has real algorithms and more tests.

## Items in this phase

1. **Code coverage**
   - Generate coverage (OpenCppCoverage on Windows, or gcov/lcov on Linux)
   - Optional upload to Codecov / Coveralls (free for open source)
   - Track coverage trends; no hard threshold required at first

2. **Sanitizers**
   - AddressSanitizer / UndefinedBehaviorSanitizer on at least one CI job (usually easier on Ubuntu)
   - Run tests under sanitizers

3. **Additional static analysis / hardening**
   - Extra clang-tidy checks
   - Optional Cppcheck
   - Consider enabling more aggressive warning levels

4. **Optional extra platforms**
   - Ubuntu job (very cheap on public repos)
   - macOS if free minutes allow and value is clear

## Acceptance criteria
- [ ] Coverage reporting available
- [ ] At least one sanitizer job running tests
- [ ] Phase 1 and Phase 2 remain green
- [ ] Documented as optional/advanced quality measures

## Notes
Phase 3 is valuable but not blocking for early development. It should be introduced after the core 2D data structures and a basic registration pipeline exist and have meaningful test coverage.
