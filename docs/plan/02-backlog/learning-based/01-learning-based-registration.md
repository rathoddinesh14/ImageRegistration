# Task: Learning-based / Deep Learning Registration Methods

**Status**: todo (future phase)  
**Component**: learning-based  
**Priority**: Low (after classical 2D foundation is solid)

## Goal
Explore and eventually support learning-based image registration approaches alongside the classical optimization-based pipeline.

## Scope (high-level)

Possible directions (to be refined later):

- Supervised / unsupervised deep registration networks (e.g. VoxelMorph-style, Transformer-based, etc.)
- Hybrid approaches (learning a metric or a deformation model that is then refined by classical optimization)
- Learned interpolators or feature extractors that plug into the existing `Metric2D` / `Interpolator2D` interfaces
- Inference-only support (load a pre-trained model and run it) vs. training support inside the library

## Design constraints (must respect existing ADRs)

- Core library stays free of heavy DL framework dependencies (PyTorch, TensorFlow, ONNX Runtime, etc.) by default.
- Any learning-based functionality should be an **optional** component or separate target.
- Prefer clean interfaces so a learned transform or metric can be used interchangeably with classical ones where it makes sense.
- Do not force the classical pipeline to depend on neural networks.

## Acceptance criteria (when we reach this phase)

- [ ] Clear ADR deciding which DL framework (if any) is allowed as an optional dependency
- [ ] At least one reference implementation or integration path documented
- [ ] Interfaces that allow a learned model to act as a `Transform2D` or contribute to a `Metric2D`
- [ ] Documentation of limitations (training vs inference, GPU requirements, model format, etc.)
- [ ] Unit / integration tests for the chosen integration path

## Notes

- This is deliberately placed in the backlog as a **future** item.
- Classical (optimization-based) 2D registration remains the primary focus until the core data structures, transforms, metrics, and registration orchestrator are mature.
- Learning-based methods can later benefit from the same `Image2D`, `Transform2D`, and geometry abstractions we are building now.

## Related

- Evolution roadmap (`docs/plan/00-vision/evolution-roadmap.md`)
- Non-goals (core stays free of heavy optional dependencies)
