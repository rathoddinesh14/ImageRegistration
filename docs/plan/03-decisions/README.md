# Architecture Decision Records

This directory records durable architectural decisions for ImageRegistration.

## Index

| ADR | Decision | Status | Related work |
|---|---|---|---|
| [001](001-use-eigen.md) | Use Eigen for linear algebra | Existing | Core |
| [002](002-testing-framework-catch2.md) | Use Catch2 for testing | Existing | Infrastructure |
| [003](003-coding-conventions.md) | Coding conventions | Existing | Engineering |
| [004](004-pure-interfaces-first.md) | Pure abstract interfaces first | Existing | Architecture |
| [005](005-license-mit.md) | MIT license | Existing | Project |
| [006](006-coding-standards.md) | Coding standards | Existing | Engineering |
| [007](007-error-handling-strategy.md) | Hybrid error handling | Existing | Core |
| [008](008-parametric-transform2d.md) | Parametric transform sub-interface | Existing | Registration |
| [009](009-reconstruction-domain-boundary.md) | Reconstruction as a peer domain | Accepted | DOC-02 / #34 |
| [010](010-projection-data-semantics.md) | Explicit projection-data semantics | Accepted | DOC-03 / #35 |
| [011](011-parallel-beam-geometry-contract.md) | Separate acquisition geometry contract | Accepted | DOC-04 / #36 |
| [012](012-discrete-projection-adjoint-contract.md) | Discrete forward/adjoint contract | Accepted | DOC-05 / #37 |
| [013](013-tiered-numerical-validation.md) | Tiered numerical validation | Accepted | DOC-06 / #38 |

## Conventions

- Use the next available zero-padded number; never reuse an existing number.
- Each ADR records context, alternatives, decision, consequences, status, date, and related documents.
- `Accepted` means the decision is the current project baseline; changing it requires a superseding ADR.
- `Proposed` means review is still required; `Deprecated` or `Superseded` records a later change.
- ADRs record decisions, not implementation status. A documented decision does not imply its proposed types or algorithms have been implemented.

## Dependency and build decisions

DOC-07 introduces no additional reconstruction-specific dependency decisions. Eigen, Catch2, and C++20 are already governed by ADRs 001, 002, and 006; the repository's CMake strategy is covered by the existing build documentation. Repeating those decisions would create competing sources of truth.
