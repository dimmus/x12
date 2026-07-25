# ADR-0009: meson.options + X12-LEVEL hierarchical security

## Status

Accepted — 2026-07-25

## Context

Meson ≥ 1.1 prefers `meson.options` over `meson_options.txt`. G1 requires hierarchical levels and a deny-keylog demonstration (ADR-0003, ADR-0007).

## Decision

1. Rename project options file to **`meson.options`** and require **`meson_version: '>=1.1.0'`**.
2. Implement built-in **`X12-LEVEL`** mediation on XACE when `xace` is enabled:
   - Levels: `sandbox` < `user` < `full`
   - Default: **full** (`-client-level`)
   - Test helper: `-sandbox-clients <indices>`
   - Sandbox: deny keyboard `DixReadAccess` / `DixGrabAccess`; deny receive on foreign windows
3. Gate via `tests/security/run_deny_keylog.sh` inside the legacy suite.

## Consequences

- Meson &lt; 1.1 cannot configure the tree (Ubuntu 24.04 ships 1.3.x — OK).
- Auth-token / polkit / compositor grant UX remains future work; CLI + index force cover G1.

## References

- ADR-0003, ADR-0007, Meson 1.1 release notes
