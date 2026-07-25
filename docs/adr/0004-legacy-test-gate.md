# ADR-0004: Legacy test gate

## Status

Accepted — 2026-07-25

## Context

Stakeholder directive: **all legacy tests should pass through.** Without a gate, compatibility claims will rot as X12 features land.

## Decision

1. CI must run a `legacy` meson test suite on every PR once the server tree exists.
2. Regressions block merge.
3. Skipping or deleting a legacy test requires a new ADR + stakeholder acknowledgment in `docs/DECISIONS.md`.
4. Until the full X11R8 corpus is imported, `tests/legacy/` holds the harness contract and placeholders.
5. New features need at least one positive test; security features need a negative (deny) test.

## Consequences

- Feature velocity limited by suite health — accepted tradeoff.
- Importing X11R8/XCB tests is an early implementation milestone.

## Alternatives

- Best-effort manual testing — rejected.
- Legacy suite only on releases — rejected (too late for regressions).
