# Backlog index

Granular tasks for ImageRegistration. Status is inside each file.

## Suggested order (Phase 1 foundation → first registration)

1. **Transforms:** Identity2D → **Rigid2D** → Affine2D → (optional) parameter interface  
2. **Interpolator:** interface → NearestNeighbor → Linear  
3. **Metric:** interface → MeanSquares  
4. **Resampler:** interface → ImageResampler2D  
5. **Errors:** Error types → `ir::Expected` (existing infra tasks)  
6. **RegistrationResult** (existing) → Registration interface → concrete method  
7. **Optimizer:** interface → simple GD  
8. **IO / examples:** PGM, read PNG, synthetic example, writer interface  

## Folders

| Folder | Focus |
|--------|--------|
| `core/` | Types, conventions, examples |
| `transform/` | Spatial transforms |
| `interpolator/` | Continuous sampling |
| `metric/` | Similarity measures |
| `resampler/` | Warp moving → fixed grid |
| `optimizer/` | Parameter search |
| `registration/` | Orchestration + results |
| `io/` | Optional read/write |
| `infrastructure/` | CI, quality, Expected |
| `learning-based/` | Future / optional |

Skip `learning-based` until classic pipeline works.
