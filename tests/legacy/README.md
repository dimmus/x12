# Legacy test harness (contract)

This directory is the **pass-through gate** described in [`docs/LEGACY_TESTS.md`](../../docs/LEGACY_TESTS.md).

## Status

Harness placeholder — server/protocol code not yet imported.  
When the X12 / X11 compatibility server lands, this suite becomes a required CI target:

```sh
./tests/legacy/run.sh
# or
meson test -C build --suite legacy
```

## Planned contents

| Path | Purpose |
|---|---|
| `run.sh` | Entry point: start Xvfb, run corpus, collect junit/log |
| `smoke/` | Minimal protocol bring-up (create/map window, events) |
| `xauth/` | Imported/adapted from `dimmus/X11R8` xauth scripts |
| `xcb/` | Imported/adapted from `dimmus/XCB` |
| `apps/` | xterm/xclock/twm smoke under virtual framebuffer |

## Rules

- Do not delete failing coverage to go green — fix or ADR.
- New skips need an issue link in the skip reason.
- Security/capability tests are separate (`tests/security/` later) and do not replace this suite.
