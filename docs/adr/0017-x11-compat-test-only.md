# ADR-0017: X11 is compat/test surface only

## Status

Accepted — 2026-07-25 (post-G1 product clarification)

## Context

G1 reused an X11R8 tree so existing apps (xterm, toolkits) and the B2
corpus keep working. That does **not** mean X12’s product goal is “best
X11 server.” Investing in deep X11 bring-up (instrumenting xkbcomp under
ASan, expanding classic DDX surface, PresentPixmap parity) pulls effort
away from the native path.

## Decision

1. **Native product path** = X12 protocol + X12-SURFACE + hierarchical
   levels + built-in compositor. New features land here first.
2. **X11 / XCB classic wire** remains as a **compatibility and test**
   surface so user apps and the locked B2 corpus keep working
   (ADR-0004 still holds — corpus skips still need ADR + Dimmus ack).
3. **Do not** expand X11 as a competitive feature set (no X11-first
   HiDPI, no X11-first security story beyond what apps need to run).
4. **CI / tooling:** prefer X12 smokes (`tests/x12/`) for hardening
   (ASan/fuzz). For ASan Xvfb runs, use **system** `xkbcomp`
   (`-Dxkb_bin_dir=/usr/bin`) rather than ASan-instrumented in-tree
   xkbcomp (ODR noise with libX11).
5. Long-term, overlapping X11 extensions may move behind shims or
   shrink once app tests no longer need them (still gated by B2).

## Consequences

- Engineering priority: SURFACE/compositor/levels over classic X11 polish.
- Legacy suite stays green as a **user-app regression net**, not as the
  identity of the project.
- Docs/README stop implying “better X.Org” as the north star.

## References

- ADR-0004 (B2 gate), ADR-0007 (G1), ADR-0010 (SURFACE), ADR-0016
- `docs/LEGACY_TESTS.md`, `tests/x12/`
