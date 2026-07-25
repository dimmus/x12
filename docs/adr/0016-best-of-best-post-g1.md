# ADR-0016: Post-G1 “best of the best” vertical slice

## Status

Accepted — 2026-07-25 (after G1 tag)

## Context

G1 proved a locked demo path. The honest gap analysis called for:
real dmabuf/DRI3 import, a fuller request matrix, XML-driven wire
constants + fuzz/ASAN gates, and a non-Xvfb bring-up.

## Decision

1. **X12-LEVEL request matrix** — sandbox/user/full enforced via XACE
   resource, selection, property, send, ext, and widened device hooks;
   proven by `tests/security/deny_matrix.c` (+ existing deny_keylog).
2. **DRI3-backed CreateSurface** when the screen has
   `pixmap_from_fds` / `pixmap_from_fd`; Xvfb keeps mmap LINEAR fallback.
   Present marks Damage after compose.
3. **XML→wire codegen** — `safe/x12-proto/tools/gen_wire.py` emits opcodes
   / sizes; `./tests/safe_proto/run_fuzz.sh` stress-decodes random buffers.
4. **CI** — keep `build-and-legacy` as B2 gate; add `asan-x12` and
   `xephyr-smoke` jobs.
5. **Vulkan Present** — lavapipe required by default
   (`X12_ALLOW_CPU_PRESENT=1` to override).
6. **Still deferred:** full drm_syncobj/MSC timeline, KMS atomic flips on
   real hardware, XML→full Rust decode codegen (sizes/opcodes first).

## Consequences

- Security story is a published matrix with tests, not a single keylog demo.
- Glamor/Xephyr/Xorg screens can take the DRI3 import path without a
  separate SURFACE ABI.
- ASAN/fuzz failures block PRs once jobs are green.

## References

- ADR-0003, ADR-0011, ADR-0013, ADR-0014, ADR-0015
- `docs/X12_LEVEL_MATRIX.md`, `docs/ROADMAP.md`
