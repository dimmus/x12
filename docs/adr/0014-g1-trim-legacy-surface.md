# ADR-0014: Trim G1 surface area (keep B2 gate)

## Status

Accepted — 2026-07-25 (post–step 7 rethink)

## Context

After the X12-SURFACE compositor landed (ADR-0013), the tree still defaulted
on nested servers, toy demos, font server, Intel video driver, and obscure
proto (Xprint/XEvIE/XWayland headers) that G1 never exercises. Docs still
described the surface path as a stub. The locked B2 corpus must stay (ADR-0004).

## Decision

1. **Keep** `./tests/legacy/run.sh` as the merge gate for the B2 corpus.
2. **Split** X12-native smokes into `./tests/x12/run.sh` (still invoked from
   the legacy harness so CI remains one job).
3. **Default-off** for G1 builds: `xnest`, `xephyr`, `build-font-server`,
   `build-video-intel`, toy demos (`ico`/`maze`/…), `xprint`, `xevie`.
4. **Stop shipping** `xwaylandproto.h` / man text unless `build-xproto-legacy`
   (XWayland DDX remains a non-goal).
5. **Honest G1 capabilities:** advertise `Modifiers` only; reject Present
   syncobj/timeline fields while Syncobj capability is off.
6. **Do not** delete DRI3/Present, Render, or the B2 corpus — legacy clients
   and ADR-0004 still require them.

## Consequences

- Faster default/CI configures; nested DDX and toy demos remain opt-in.
- Docs stop calling SURFACE a stub; Present wire layout stays (reserved pads
  for future sync/MSC) without claiming PresentPixmap identity.
- Further corpus deletions still need ADR + Dimmus ack.

## References

- ADR-0004, ADR-0010, ADR-0013
- `meson.options`, `tests/x12/run.sh`, `docs/LEGACY_TESTS.md`
