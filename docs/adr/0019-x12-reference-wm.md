# ADR-0019: X12 reference window manager (xwm)

## Status

Accepted — 2026-07-25

## Context

X12’s built-in compositor (X12-SURFACE) owns buffer scanout and sync. It does
**not** replace a client window manager (titlebars, focus, ICCCM
SubstructureRedirect). Stakeholders want a **fully functional twm-class WM**
in-tree for sessions and demos without shipping a full desktop environment
(ADR-0002 H4). Classic `twm` is already vendored and built by default.

## Decision

1. **xwm** is the X12 reference WM face: a thin launcher over the in-tree
   **twm** engine with `system.xwmrc` (portable fonts, root menu, move/resize/
   iconify/focus bindings).
2. WMs remain **clients** (ADR-0005). xwm must run at client level `user` or
   `full` — not `sandbox` (ADR-0018).
3. Session entry points: `xwm`, `$datadir/X11/xwm/x12-xinitrc`, and
   `scripts/x12-session.sh`. Smoke: `tests/x12/run_wm.sh` (reparent + WM_STATE).
4. No EWMH/DE scope in this ADR. External WMs (Window Maker, etc.) remain
   first-class consumers of the server.

## Consequences

- G1+ demos can show a real managed desktop without a separate DE.
- Product still is not “X12 ships a desktop environment.”
- Future EWMH / native-Rust WM would be a new ADR, not a silent expansion.

## References

- ADR-0002, ADR-0005, ADR-0017, ADR-0018
- `app/xwm/`, `app/twm/`, `docs/ROADMAP.md`
