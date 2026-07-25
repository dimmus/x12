# ADR-0004: Legacy test gate

## Status

Accepted — 2026-07-25 (corpus locked by QUESTIONS B1, B2)

## Context

Stakeholder directive: **all legacy tests should pass through.** Without a gate, compatibility claims will rot as X12 features land.

## Decision

1. CI must run a `legacy` meson test suite on every PR once the server tree exists.
2. Regressions block merge.
3. Skipping or deleting a legacy test requires a new ADR + **Dimmus** acknowledgment in `docs/DECISIONS.md`.
4. **Corpus (locked):** from `dimmus/X11R8` — meson test suite, xauth scripts, x11perf, xcmstest.
5. Until that corpus is imported, `tests/legacy/` holds the harness contract and placeholders.
6. New features need at least one positive test; hierarchical security needs a negative (deny keylog) test for G1.

## Consequences

- Feature velocity limited by suite health — accepted tradeoff.
- Importing X11R8/XCB tests is an early implementation milestone.

## Alternatives

- Best-effort manual testing — rejected.
- Legacy suite only on releases — rejected (too late for regressions).
