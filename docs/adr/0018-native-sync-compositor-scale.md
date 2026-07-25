# ADR-0018: Native Present sync, compositor clock, dmabuf honesty, scale

## Status

Accepted — 2026-07-25 (post-G1 native next)

## Context

After ADR-0016/0017, Present still returned `BadImplementation` for
ImportSyncobj, ignored MSC, and composed with immediate CopyArea only.
Xephyr nest smoke checked QueryVersion, not dmabuf import. Scaling and
full XML→Rust decode remained open.

## Decision

1. **Software timeline syncobj on Xvfb** — ImportSyncobj/FreeSyncobj are
   real resources. Present honors acquire/release points (release advances
   the timeline). Capability.Syncobj is advertised. DRM syncobj ioctls land
   when a DDX provides `import_syncobj` (not required for Xvfb CI).
2. **Compositor MSC queue** — ~60Hz fake clock; `target_msc` / divisor /
   remainder / Async drive queueing; CompleteNotify carries ust/msc;
   pending presents for a screen flush in stacking order with Damage.
   CopyArea remains the Xvfb scanout path; atomic KMS flips stay DDX work.
3. **Live DRI3 caps** — Multiplane only when `dri3_screen_priv` has
   `pixmap_from_fds` / `pixmap_from_fd`. Xephyr glamor registers
   `dri3_screen_init` so nested import can leave the mmap path when the
   host EGL/GBM stack can import. No `/dev/dri` in CI ⇒ Xvfb mmap stays.
4. **X12-SCALE stub** — QueryVersion / GetScale (unity 16.16) / OptIn.
   Toolkit must opt in; server does not silently blur-scale (ADR-0005 D4).
5. **Level matrix widen** — sandbox denies all device DixRead/Grab/Write
   (XI2 pointer + keyboard) and XFixes cursor DixRead; document sandbox ≠ WM.
6. **XML→Rust decode codegen** — `gen_wire.py` emits `generated_decode.rs`
   for fixed requests; CreateSurface/Present keep hand validators.

## Consequences

- `tests/surface/sync_present` and `scale_stub` join `./tests/x12/run.sh`.
- Real KMS atomic scanout and DRM timeline ioctls remain explicit follow-ons
   once a DRM-capable DDX environment exists.

## References

- ADR-0005, ADR-0010, ADR-0013, ADR-0016, ADR-0017
- `server/Xext/x12surface.c`, `x12scale.c`, `docs/X12_LEVEL_MATRIX.md`
