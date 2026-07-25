# ADR-0002: Compatibility-first evolution

## Status

Provisional — 2026-07-25 (awaiting QUESTIONS B3, B4, A1)

## Context

The org already maintains `X11R8`, `XCB`, Motif, EFL, Window Maker, CDE. A greenfield break would orphan that stack. Stakeholder requires legacy tests to keep passing.

## Decision (provisional)

1. X12 development proceeds as an **evolution** of the X Window System, not a Wayland reimplementation.
2. Existing X11 clients and the X11R8-derived test corpus remain supported.
3. New X12 capabilities (buffers, HDR, capabilities) are additive; legacy paths stay until tests and ADR retire them.
4. Prefer dual-path server internals: **legacy protocol engine** + **native X12 surface/capability engine**, sharing DRM/KMS backend.

## Consequences

- Slower pure elegance; higher adoption chance for Motif/EFL/WMs.
- Protocol IDL must describe both legacy and native messages.
- Security model must apply to legacy clients without requiring immediate porting (via namespaces/capabilities at connection time).

## Alternatives

- Hard cut to new protocol only — rejected under legacy-test constraint.
- Wayland compositor + XWayland — rejected as product identity (X11R8 already drops XWayland).

## References

- `dimmus/X11R8` README: monolithic, no XWayland/XWin/XQuartz
- `docs/LEGACY_TESTS.md`
