# ADR-0007: Milestone G1, governance, and deferred scope

## Status

Accepted — 2026-07-25 (QUESTIONS A2, E*, G1–G3, H1–H5)

## Context

Strategy questions locked product shape, first demo, ownership, and what is explicitly out of scope. Remoting questions were skipped.

## Decision

### Governance

- **Decision owner:** Dimmus (G3). ADR status changes that alter Accepted architecture require Dimmus acknowledgment in `docs/DECISIONS.md`.

### First public milestone (G1)

Demonstrate:

1. **Xvfb** (or equivalent virtual framebuffer server path) running
2. **xterm** (legacy client) working
3. **Capability deny keylog** — a restricted-level client cannot snoop keys destined for another client
4. **One Vulkan client** presenting via the API-agnostic dmabuf native path into the built-in compositor

Legacy X11R8 corpus (ADR-0004 / B2) must remain passable as the suite is imported; G1 does not waive the legacy gate.

### Deferred / skipped

- **Remoting / network transparency (E1–E3): deferred.** No remoting transport ADR in this round. Local modern Linux desktop is the v1 focus (A2). Revisit remoting only under a new ADR.

### Confirmed non-goals (H1–H5)

| ID | Non-goal |
|---|---|
| H1 | Not a Wayland compositor implementation |
| H2 | No XWayland |
| H3 | No XWin / XQuartz in v1 |
| H4 | Not a full desktop environment |
| H5 | No guarantee of wire-compat with every obscure historical extension |

## Consequences

- Engineering sequence: import/adapt X11R8 baseline → hierarchical mediation hooks → Xvfb+xterm green → sandbox keylog deny test → dmabuf/Vulkan client sample.
- Remoting-related text in older drafts is informational only until a future ADR.

## Alternatives

- Remoting as v1 pillar — not chosen (E skipped).
- Broader G1 (HDR, fractional scale, multi-GPU) — deferred; HDR is v2 (D3), scaling is toolkit-opt-in (D4).

## References

- `docs/QUESTIONS.md` Answers 2026-07-25
- ADR-0003 (hierarchical levels), ADR-0005 (compositor + dmabuf)
