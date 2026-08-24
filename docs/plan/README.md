# ImageRegistration — Project Plan

This folder contains the living plan for the library.

## Structure

| Folder | Purpose |
|--------|---------|
| `00-vision/` | High-level goals, principles, non-goals, and roadmap |
| `01-implemented/` | What has already been delivered |
| `02-backlog/` | Granular, independent tasks still to be done |
| `03-decisions/` | Architecture Decision Records (ADRs) |

## How we work

1. Data structures & pure interfaces first.
2. Algorithms second.
3. Reuse mature libraries (Eigen, Catch2, …) — do not reinvent the wheel.
4. Keep the core free of I/O, visualization, and heavy frameworks.
5. Grow 2D → 3D → time-series in controlled phases.

## Status legend (used in backlog files)

- `todo`
- `in-progress`
- `done`
- `blocked`
