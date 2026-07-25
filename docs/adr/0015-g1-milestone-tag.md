# ADR-0015: Tag milestone G1

## Status

Accepted — 2026-07-25 (ROADMAP step 8)

## Context

ROADMAP step 8: tag G1 when the demo script and legacy suite are green.
ADR-0007 defined G1 as Xvfb + xterm + deny keylog + one Vulkan client, with
the B2 corpus still gating merges.

## Decision

1. **Git tag** `g1` (annotated) marks the milestone on this line of development.
2. **Evidence** is recorded in [`docs/G1.md`](../G1.md); reproduction is
   `./tests/legacy/run.sh` (B2 + `./tests/x12/run.sh`) and
   `./tests/safe_proto/run.sh`.
3. **Meson project version** becomes `12.0.0-g1` (was `12.0.0-dev`).
4. G1 does **not** waive ADR-0004; further corpus skips still need ADR + Dimmus ack.
5. Post-G1 work starts from the “Explicitly after G1” list in ROADMAP (no scope
   expansion into H1–H5 non-goals).

## Consequences

- Public/reference point for “G1 green” without implying a full desktop or KMS
  bring-up.
- Next tags (e.g. desktop/KMS) require their own ADR + ROADMAP entries.

## References

- ADR-0004, ADR-0007, ADR-0013, ADR-0014
- `docs/G1.md`, `docs/ROADMAP.md`
