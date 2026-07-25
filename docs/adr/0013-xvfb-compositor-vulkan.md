# ADR-0013: Xvfb compositor path + Vulkan dmabuf client (G1)

## Status

Accepted — 2026-07-25 (ROADMAP step 7)

## Context

G1 requires one Vulkan client presenting via X12-SURFACE into a built-in
compositor. Xvfb has no DRI3/`pixmap_from_fds` hooks; glamor/GBM is out of
scope for the Xvfb demo.

## Decision

1. **Xvfb compositor:** mmap LINEAR dmabuf/memfd planes into an fb pixmap
   (`ModifyPixmapHeader`); `Present` = `CopyArea` into the attached window;
   emit XGE Complete/Idle notifies.
2. **Supported G1 formats:** `DRM_FORMAT_XRGB8888` / `ARGB8888`,
   `DRM_FORMAT_MOD_LINEAR` (or INVALID→linear). Syncobj capability off.
3. **Decode-first retained** (ADR-0012); `SetReqFds` peeked before Rust decode.
4. **Vulkan client** (`tests/surface/vk_present.c`): lavapipe fills a
   host-visible buffer, exports via **memfd** (CPU-mappable stand-in for
   LINEAR dmabuf on Xvfb), then CreateSurface → Attach → Present.
5. **CI:** install `mesa-vulkan-drivers`; run `./tests/surface/run_vk_present.sh`
   in the legacy suite (falls back to `--cpu` if Vulkan init fails).

## Consequences

- Real KMS/GBM import remains a follow-on; this satisfies G1 on Xvfb.
- Proprietary NVIDIA / tiled modifiers are explicitly out of G1.
- Memfd is protocol-compatible with the mmap importer; lavapipe proves the
  Vulkan producer side.

## Alternatives

- Wire Xvfb to glamor/GBM — rejected for G1 scope/complexity.
- Require PresentPixmap + DRI3 — rejected (ADR-0010 native SURFACE path).

## References

- ADR-0005, ADR-0010, ADR-0012
- `server/Xext/x12surface.c`, `tests/surface/vk_present.c`
