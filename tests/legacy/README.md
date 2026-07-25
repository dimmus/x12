# Legacy test harness (B2 corpus gate)

Pass-through gate for the **X11R8** corpus — see [`docs/LEGACY_TESTS.md`](../../docs/LEGACY_TESTS.md)
and [ADR-0004](../../docs/adr/0004-legacy-test-gate.md).

## Locked corpus (B2)

- X11R8 meson unit tests (selected non-piglit targets)
- xauth scripts
- x11perf
- xcmstest

## Entry point

```sh
./tests/legacy/run.sh
```

This runs the B2 corpus, then delegates to [`../x12/run.sh`](../x12/run.sh) for
X12-native G1 smokes (deny_keylog, surface QueryVersion, vk_present). Those
smokes are **not** part of the locked corpus; they ride along so G1 stays green
in one CI job.

## Rules

- Do not delete failing B2 coverage to go green — fix or ADR + Dimmus ack.
- New corpus skips need an issue link in the skip reason.
- X12-only tests live under `tests/{security,surface,x12}/`; do not rename the
  B2 gate away without a new ADR.
