# Legacy Test Pass-Through Policy

**Status:** Accepted  
**Related ADR:** [adr/0004-legacy-test-gate.md](adr/0004-legacy-test-gate.md)  
**Corpus locked by:** QUESTIONS B1, B2 (2026-07-25)  
**Trim note:** [adr/0014-g1-trim-legacy-surface.md](adr/0014-g1-trim-legacy-surface.md)  
**Scope:** X11 corpus is an **app/compat regression net**, not the product identity ([ADR-0017](adr/0017-x11-compat-test-only.md)).

## Rule

> **All legacy tests must pass through.**  
> No X12 change may land if it regresses the designated legacy corpus.

“Pass through” means:

1. Legacy tests remain executable in CI and locally.
2. Failures block merge (non-optional).
3. Removals or skips require an explicit ADR + Dimmus acknowledgment in [DECISIONS.md](DECISIONS.md).
4. New X12 features must not break X11-compatible behavior covered by the corpus.

## In-scope corpus (locked B2)

From the imported X11R8 tree:

| Component | How it runs |
|---|---|
| Meson unit tests | Selected non-piglit targets in `./tests/legacy/run.sh` |
| xauth scripts | `app/xauth/tests` via `XAUTH_BIN` |
| x11perf | `demo/x11perf` under Xvfb |
| xcmstest | `demo/xcmstest` under Xvfb |

## Out of scope (unless a future ADR adds them)

- XCB standalone unit suites (not selected in B2)
- Motif / EFL / Window Maker smoke tests
- XWayland, XWin, XQuartz
- Distro packaging tests
- Obscure historical extension conformance beyond what X11R8 already runs (H5)
- Toy demos (ico/maze/puzzle/…) — default-off (ADR-0014); not B2

## X12-native smokes (not B2)

`./tests/x12/run.sh` — deny_keylog, surface QueryVersion, vk_present. Invoked
after the corpus by `./tests/legacy/run.sh` so one CI job covers G1.

## CI expectations

```text
./tests/legacy/run.sh   # B2 corpus + X12 smokes
```

There is no Meson `suite: 'legacy'` yet; the shell harness is the gate.

- Prefer Xvfb for headless CI (aligns with G1).
- Silent skips forbidden; quarantines need an issue link.

## Local developer workflow

```sh
./tests/legacy/run.sh     # full gate
./tests/x12/run.sh        # X12-only smokes
./tests/safe_proto/run.sh # Rust decode + IDL drift
```
