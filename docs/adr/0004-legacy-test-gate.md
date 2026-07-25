# ADR-0004: Legacy test gate

## Status

Accepted — 2026-07-25 (corpus locked by QUESTIONS B1, B2)

## Context

Stakeholder directive: **all legacy tests should pass through.** Without a gate, compatibility claims will rot as X12 features land.

## Decision

1. CI must run the B2 corpus via `./tests/legacy/run.sh` on every PR (shell harness; no Meson `suite: legacy` yet).
2. Regressions block merge.
3. Skipping or deleting a legacy test requires a new ADR + **Dimmus** acknowledgment in `docs/DECISIONS.md`.
4. **Corpus (locked):** from `dimmus/X11R8` — meson unit tests, xauth scripts, x11perf, xcmstest.
5. X12-native G1 smokes live in `./tests/x12/run.sh` and are invoked after the corpus (ADR-0014); they are not B2.
6. New features need at least one positive test; hierarchical security needs a negative (deny keylog) test for G1.

## Consequences

- Feature velocity limited by suite health — accepted tradeoff.
- Corpus is imported and gated; further deletions still need ADR + ack.

## Alternatives

- Best-effort manual testing — rejected.
- Legacy suite only on releases — rejected (too late for regressions).
