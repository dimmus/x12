# Legacy Test Pass-Through Policy

**Status:** Accepted  
**Related ADR:** [adr/0004-legacy-test-gate.md](adr/0004-legacy-test-gate.md)  
**Corpus locked by:** QUESTIONS B1, B2 (2026-07-25)

## Rule

> **All legacy tests must pass through.**  
> No X12 change may land if it regresses the designated legacy corpus.

“Pass through” means:

1. Legacy tests remain executable in CI and locally.
2. Failures block merge (non-optional).
3. Removals or skips require an explicit ADR + Dimmus acknowledgment in [DECISIONS.md](DECISIONS.md).
4. New X12 features must not break X11-compatible behavior covered by the corpus.

## In-scope corpus (locked)

From `dimmus/X11R8` only:

| Component | Examples |
|---|---|
| Meson test suite | Targets defined by X11R8 `meson.build` / test harness |
| xauth scripts | `app/xauth/tests/*` |
| x11perf | `demo/x11perf` harness / do_tests |
| xcmstest | `demo/xcmstest` |

Import plan: vendor or submodule X11R8 pieces under `tests/legacy/` (or run against an X11R8 checkout) and wrap with `./tests/legacy/run.sh`.

## Out of scope (unless a future ADR adds them)

- XCB standalone unit suites (not selected in B2)
- Motif / EFL / Window Maker smoke tests
- XWayland, XWin, XQuartz
- Distro packaging tests
- Obscure historical extension conformance beyond what X11R8 already runs (H5)

## CI expectations

```text
meson test -C build --suite legacy
# or
./tests/legacy/run.sh
```

- Prefer Xvfb for headless CI (aligns with G1).
- ASAN/UBSAN advisory until a green baseline exists, then gate.
- Silent skips forbidden; quarantines need an issue link.

## Local developer workflow (target)

```sh
./tests/legacy/run.sh
meson test -C build --suite legacy
```
