# Legacy test harness (contract)

Pass-through gate for the **X11R8** corpus — see [`docs/LEGACY_TESTS.md`](../../docs/LEGACY_TESTS.md).

## Locked corpus (B2)

- X11R8 meson test suite
- xauth scripts
- x11perf
- xcmstest

## Status

Harness placeholder until the X11R8-derived server tree is imported.

```sh
./tests/legacy/run.sh
# later:
meson test -C build --suite legacy
```

## Rules

- Do not delete failing coverage to go green — fix or ADR + Dimmus ack.
- New skips need an issue link in the skip reason.
- G1 security demo (`tests/security/` later) complements this suite; it does not replace it.
