# Task: Read PNG into Image2D

**Status**: todo  
**Component**: io  
**Priority**: Medium  
**Depends on**: Image2D, optional geometry defaults

## Goal
Decode grayscale (or convert RGB→gray) PNG to `Image2D` using stb_image or similar, optional IO target only.

## Acceptance criteria
- [ ] `readPng` / `PngReader` API
- [ ] Document geometry defaults (unit spacing, origin at pixel center of 0,0)
- [ ] Round-trip test with writePng on synthetic image
