# ADR-0008: Import X11R8 as the X12 code baseline

## Status

Accepted — 2026-07-25 (Dimmus / ROADMAP step 1)

## Context

G1 requires a working Xvfb + xterm path and the locked legacy corpus from `dimmus/X11R8`. Strategy chose an independent line that speaks X11 natively (ADR-0002) with a hybrid memory-safe front-end later (ADR-0006).

## Decision

1. **Vendor-copy** the `dimmus/X11R8` meson tree into this repository root (not a git submodule), so X12 can evolve independently.
2. Record provenance: imported from commit `9b6e8f9cc170d88a5ed5fa7687e8876c7ee2fb02` (X11R8 `main` at import time). See `docs/vendor/X11R8-README.md`.
3. Preserve X12 strategy docs under `docs/`, `tests/legacy/`, top-level `README.md`, and MIT/X11 `LICENSE`.
4. Rename the meson project to `x12` / `12.0.0-dev` while keeping the X11 protocol lineage.
5. Default-enable options needed for the legacy gate: `build-test`, `build-demo`, `demo-xcmstest`.
6. Brand vendor strings as `X12 (Dimmus)`.
7. Apply minimal clang/build fixes required to compile on Ubuntu 24.04 + clang 18 (recorded in the import PR).

## Consequences

- Large initial tree in git; subsequent X12 work is normal commits on this tree.
- Upstream X11R8 fixes can be cherry-picked manually.
- Hierarchical security, dmabuf surfaces, and the memory-safe front-end land as follow-on commits on this baseline.

## Alternatives

- Git submodule — rejected for G1 velocity and independent evolution.
- Greenfield server — rejected; fails legacy-test constraint.

## References

- ADR-0002, ADR-0004, `docs/ROADMAP.md`, `docs/LEGACY_TESTS.md`
