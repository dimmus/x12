# AGENTS

## Cursor Cloud specific instructions

This repository (`x12`) is currently **documentation/strategy only** — there is no
application source tree, build system, or package-managed dependencies yet. See
[`README.md`](README.md) and [`docs/ROADMAP.md`](docs/ROADMAP.md) for the plan
toward the G1 milestone.

### What can run today

- The only executable artifact is the legacy test gate: `./tests/legacy/run.sh`
  (contract described in [`docs/LEGACY_TESTS.md`](docs/LEGACY_TESTS.md)).
- It requires only `bash` (already present on the VM); there is nothing to install.
- Behavior is intentionally state-dependent:
  - No server binary under `build/` (`x12-server`, `Xvfb`, or `hw/vfb/Xvfb`)
    → prints `SKIP` and exits `0` (current default state).
  - A server binary present → exits `1` (`corpus runner not yet implemented`).

### Lint / test / build / run

- No linter, test framework, or build system is configured yet.
- "Run/test" today == `./tests/legacy/run.sh`.
- Future target (not yet wired, do not assume it exists): `meson test -C build --suite legacy`.

### Gotchas

- Do not add system build tools (meson, X11 libs, etc.) to the update script — they
  are not part of the repo yet and would be speculative.
- Legacy-test policy: never delete failing coverage to go green; skips/removals need
  an ADR + owner acknowledgment (see [`docs/LEGACY_TESTS.md`](docs/LEGACY_TESTS.md)).
