# ADR-0019: X12 reference window manager (xwm)

## Status

Accepted — 2026-07-25 (amended: from-scratch engine)

## Context

X12’s built-in compositor (X12-SURFACE) owns buffer scanout and sync. It does
**not** replace a client window manager (titlebars, focus, ICCCM
SubstructureRedirect). Stakeholders want a **fully functional twm-class WM**
in-tree for sessions and demos without shipping a full desktop environment
(ADR-0002 H4). Classic `twm` remains vendored as a separate app; the product
reference WM is a dedicated engine.

## Decision

1. **xwm** is the X12 reference WM: a **from-scratch** Xlib ICCCM engine
   (frames, titlebars, move/resize, iconify/close, click-to-focus, root menu,
   `RandomPlacement`, `WM_STATE` / `WM_PROTOCOLS`). Config is key=value
   (`system.xwmrc` / `~/.xwmrc`).
2. WMs remain **clients** (ADR-0005). xwm must run at client level `user` or
   `full` — not `sandbox` (ADR-0018).
3. Session entry points: `xwm`, `$datadir/X11/xwm/x12-xinitrc`, and
   `scripts/x12-session.sh`. Smoke: `tests/x12/run_wm.sh` (reparent + WM_STATE).
4. In-tree `twm` is optional/compat, not the xwm implementation.
5. No EWMH/DE scope in this ADR. External WMs remain first-class consumers of
   the server. EWMH would be a new ADR.

## Consequences

- G1+ demos can show a real managed desktop without a separate DE.
- Product still is not “X12 ships a desktop environment.”
- xwm is maintainable as a small C codebase under `app/xwm/`; twm is not a
  runtime dependency of xwm.

## References

- ADR-0002, ADR-0005, ADR-0017, ADR-0018
- `app/xwm/`, `docs/ROADMAP.md`
