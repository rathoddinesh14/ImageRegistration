# Task: IImageWriter interface (optional modularization)

**Status**: todo  
**Component**: io  
**Priority**: Low  
**Depends on**: PngWriter (done)

## Goal
Introduce a narrow writer interface/concept so PngWriter/PgmWriter share a common shape (as discussed in modular IO design).

## Acceptance criteria
- [ ] `IImageWriter` or C++20 concept under `include/ir/io/`
- [ ] `PngWriter` implements it without breaking existing `writePng` / facade
- [ ] Tests still green
