# ADR-0003: Hierarchical security model

## Status

Accepted — 2026-07-25 (QUESTIONS C1–C5, G1)

## Context

Classic X11 ambient trust enables keylogging, input injection, and screen theft. Wayland-style total isolation breaks trusted automation. Stakeholder chose hierarchical levels with a **full** default for ordinary local clients, while still requiring a milestone that demonstrates **capability deny keylog** for restricted clients.

## Decision

1. Permission model is **hierarchical levels**: `sandbox` < `user` < `full` (C1).
2. **Default** for newly connected local clients is **`full`** (C2) — preserves classic X tooling UX for trusted desktop sessions.
3. Levels may be assigned/lowered/raised via **all** grant channels (C3):
   - CLI / server flags
   - polkit-like agent
   - `.Xauthority` (or successor) tokens
   - compositor prompts
4. **Trusted automation** (macros, xdotool-class tools, accessibility) must work **without** globally disabling security (C4) — use `full` or explicit grants, not a master “turn off isolation” switch as the only path.
5. Multi-user-on-one-seat and nested servers: **in scope**, using the **most up-to-date** seat/session design available (C5) — prefer modern seatd/logind-class integration over legacy multi-seat hacks.
6. Milestone G1 requires a demo where a **non-full** (sandbox/user) client **cannot keylog** other clients — hierarchical enforcement is real, not documentary.

### Level sketch (normative intent)

| Level | Intent |
|---|---|
| `sandbox` | Own windows/resources only; no foreign input/content/selection sniff; no synthetic inject |
| `user` | Normal app interactions in the user session; limited cross-client ops per policy |
| `full` | Trusted session client / automation / WM-class tools; broad access within the seat |

Exact request matrices will be specified in a later protocol extension ADR; this ADR locks the model shape.

## Consequences

- Default-full means shipping desktops remain familiar; lockdown is opt-in per client/token.
- Security tests must cover sandbox deny paths (keylog) and full-path automation success.
- Grant UX surface is broad — need a small policy library shared by CLI, agent, auth, and compositor.

## Alternatives

- Default sandbox (Wayland-like) — rejected (C2).
- Fine-grained capabilities as the *only* primitive — rejected as primary model (C1 chose levels); fine-grained bits may still implement levels internally.
- Namespace-only (XLibre-like) as sole primitive — not chosen as primary; may still inform isolation of selections between levels.

## References

- `docs/QUESTIONS.md` Answers 2026-07-25
- Community X12 hierarchical proposal (xorg-devel, 2026-03)
- XACE / XLibre Xnamespace as implementation study material
