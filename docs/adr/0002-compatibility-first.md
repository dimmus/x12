# ADR-0002: Compatibility-first evolution (native X11 wire)

## Status

Accepted — 2026-07-25 (QUESTIONS A1, A3, B3, B4, H1–H5)

## Context

The org maintains `X11R8`, `XCB`, Motif, EFL, Window Maker, CDE. Stakeholder requires the X11R8 legacy corpus to keep passing. Product identity is an X Window System successor, not a Wayland compositor.

## Decision

1. X12 is a **protocol + server** successor (A1=a). Other WMs/toolkits adopt it; X12 is not a full desktop environment (H4).
2. Development is an **independent** `dimmus/x12` line (A3).
3. The server **speaks the X11 protocol natively**; new features land as **extensions that evolve in place** (B3=a). No separate translator shim and no dual-socket protocol split.
4. **Rebuild** against new X12 client libraries is acceptable for native features (B4). Unchanged binary ABI for every historical Xlib build is not a hard requirement.
5. Designated X11R8 legacy tests remain green (B1, B2; ADR-0004).
6. Non-goals confirmed (H1–H5): not a Wayland compositor; no XWayland; no XWin/XQuartz in v1; not a DE; no guarantee for every obscure historical extension.

## Consequences

- Protocol IDL (XML/XCB-style, ADR-0006) describes core + extensions on the X11 wire family.
- Security and buffer APIs are additive extensions, mediated for rebuilt and legacy-path clients.
- Independence means we may study XLibre/X.Org technically but do not depend on their project governance.

## Alternatives

- Dual-protocol sockets or external shim — rejected (B3).
- Wayland + XWayland — rejected (H1, H2).
- Binary-only forever without rebuild — rejected (B4).

## References

- `docs/QUESTIONS.md` Answers 2026-07-25
- `docs/LEGACY_TESTS.md`
