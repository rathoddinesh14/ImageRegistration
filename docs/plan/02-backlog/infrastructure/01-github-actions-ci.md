# Task: GitHub Actions CI

**Status**: todo  
**Component**: infrastructure  
**Priority**: High (should be done early)

## Goal
Set up continuous integration using GitHub Actions that builds the library and runs tests on every push / pull request.

## Constraints (from project owner)
- Primary target platform: **Windows** (developer only has a Windows machine)
- Must stay within **GitHub Free** tier limits (public repository)
- No paid GitHub features required

## Recommended scope for first version

### Workflow: `.github/workflows/ci.yml`

**Triggers**
- Push to `main`
- Pull requests targeting `main`

**Jobs (keep minimal for free tier)**
1. **Windows build & test** (primary)
   - Runner: `windows-latest`
   - Configure with CMake (`IR_BUILD_TESTS=ON`)
   - Build (Release)
   - Run tests with CTest / Catch2
2. Optional later: Ubuntu job (cheap on free tier for public repos) for extra coverage

**What the workflow should do**
- Checkout code
- Install / locate Eigen (vcpkg, chocolatey, or FetchContent)
- Configure + build
- Run the test suite
- Fail the job if any test fails

## Free-tier considerations
- Public repositories have very generous free minutes.
- Prefer a single Windows job at first to keep usage low and feedback fast.
- Avoid large matrices, heavy caching setups, or unnecessary services in the first version.
- No need for self-hosted runners or paid macOS/Windows minutes beyond the free allowance.

## Acceptance criteria
- [ ] `.github/workflows/ci.yml` exists and is valid
- [ ] Workflow runs successfully on `windows-latest`
- [ ] Library builds with `IR_BUILD_TESTS=ON`
- [ ] Tests are executed and the job fails if tests fail
- [ ] Status badge can be added to README later
- [ ] Documented in the plan (`01-implemented/` once done)

## Related tasks
- `testing-infrastructure.md` (Catch2 integration) — should be done together or just before this task
- Future: add Ubuntu job, clang-format check, sanitizers, coverage

## Notes
Because the primary development machine is Windows, making the CI pass on `windows-latest` first gives the highest confidence that the code works in the environment the owner actually uses.
