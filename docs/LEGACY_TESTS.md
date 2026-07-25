# Legacy Test Pass-Through Policy

**Status:** Accepted (stakeholder-stated hard constraint)  
**Related ADR:** [adr/0004-legacy-test-gate.md](adr/0004-legacy-test-gate.md)

## Rule

> **All legacy tests must pass through.**  
> No X12 change may land if it regresses the designated legacy corpus.

“Pass through” means:

1. Legacy tests remain executable in CI and locally.
2. Failures block merge (non-optional).
3. Removals or skips require an explicit ADR + replacement coverage.
4. New X12 features must not break X11-compatible behavior covered by the corpus.

## In-scope corpus (initial — refine via QUESTIONS B2)

Until stakeholder answers finalize the list, treat these as **must-not-regress** sources from the org lineage:

| Source | Examples | Import plan |
|---|---|---|
| `dimmus/X11R8` | `app/xauth/tests/*`, x11perf harness pieces, xcmstest, server smoke | Vendor or submodule + wrapper in `tests/legacy/` |
| `dimmus/XCB` | XCB protocol / binding tests as available | Wire into meson test target |
| Core protocol smoke | Xvfb start, create window, MapWindow, basic events | `tests/legacy/smoke/` |
| Selected apps | `xterm` / `xclock` / `twm` bring-up under Xvfb | scripted smoke |

Out of scope by default (aligned with X11R8 “Changes from mainstream” unless QUESTIONS reopen them):

- XWayland suites
- XWin / XQuartz
- Distro-specific packaging tests

## CI expectations

```text
meson test -C build --suite legacy
```

- Run under Xvfb (or Xephyr where needed).
- ASAN/UBSAN jobs are advisory at first, then gating once green baseline exists.
- Flaky tests must be fixed or quarantined with an issue link; silent skip is forbidden.

## Changing the corpus

Allowed only when:

1. An ADR documents why a test is obsolete under X12, and
2. Equivalent assurance exists (new test or narrowed assertion), and
3. Stakeholder approval is recorded in [DECISIONS.md](DECISIONS.md).

## Local developer workflow (target)

```sh
./tests/legacy/run.sh          # wrapper — to be implemented with server code
meson test -C build --suite legacy
```

Placeholder layout lives under `tests/legacy/` so the policy is enforceable as soon as the server tree lands.
