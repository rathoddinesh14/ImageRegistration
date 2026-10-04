# Task: PgmWriter (zero-dependency export)

**Status**: todo  
**Component**: io  
**Priority**: Low  
**Depends on**: Image2D, ExportOptions / write facade

## Goal
Write grayscale PGM for debugging without stb; register `.pgm` in `ir::io::write` facade.

## Acceptance criteria
- [ ] `PgmWriter` under `include/ir/io/`
- [ ] Facade dispatches `.pgm`
- [ ] Catch2: file header magic `P5` or `P2` as chosen
