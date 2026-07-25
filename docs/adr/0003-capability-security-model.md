# ADR-0003: Capability / hierarchical security model

## Status

Provisional — 2026-07-25 (awaiting QUESTIONS C1–C5)

## Context

Classic X11 ambient trust enables keylogging, input injection, and screen theft by any client. Wayland’s default isolation broke legitimate global tooling. XLibre’s Xnamespace (via XACE) shows a practical isolation direction. Community X12 mail (Mar 2026) proposed sandbox / user / full levels.

## Decision (provisional)

1. **Default deny** for: global raw key/button grabs, foreign window content read, synthetic input injection, unrestricted selection sniffing, overlay redirect abuse.
2. Use a **hybrid model**:
   - **Namespaces** for coarse isolation (selections, client-to-client messaging).
   - **Capabilities** for privileged operations (capture, inject, global-bind, accessibility).
   - Optional **tiers** (sandbox / user / trusted) as convenience profiles over capability sets.
3. Trusted automation and accessibility must remain possible via **explicit grant**, not by turning isolation off.
4. Auth tokens (`.Xauthority` / successor) bind a connection to a namespace + capability set.
5. Implement mediation on XACE-style hooks so legacy request paths are covered.

## Consequences

- Requires a grant agent / policy config story (UX TBD in QUESTIONS C3).
- Legacy tools like naive `xinput` sniffers will fail until granted — intentional.
- Security tests become part of CI alongside legacy functional tests.

## Alternatives

- Ambient trust forever — rejected.
- Pure Wayland-style compositor-only policy with no global tools — rejected for this lineage’s tooling needs.
- SELinux-only mediation — insufficient alone; keep as optional reinforcement.

## References

- XACE specification
- XLibre `Xnamespace` documentation
- xorg-devel “X12: A proposal for modernizing the X Window System” (2026-03)
