# ADR-0005: Rendering path and synchronization

## Status

Provisional — 2026-07-25 (awaiting QUESTIONS D1–D5)

## Context

State-of-the-art Linux display stacks converge on: client-allocated buffers, dma-buf + modifiers, atomic KMS, explicit sync, and a compositor-owned scanout path. X11’s historical server-side drawing and implicit fencing conflict with Vulkan and mixed-refresh hardware.

## Decision (provisional)

1. **Primary native path:** client renders to dmabuf-backed surfaces (Vulkan or EGL/GL); server/compositor composites and presents via atomic KMS.
2. **Explicit sync** is first-class for native clients; implicit sync retained only for legacy compatibility.
3. **TearFree / vsync-correct presentation** is the default policy where hardware supports it.
4. Classic X drawing (Render/GC core) remains for legacy clients and tests; may be implemented via Glamor-like GPU acceleration.
5. Per-output scale (including fractional) and color/HDR metadata are protocol concerns for native surfaces; legacy clients get a defined scaling policy (TBD: server-side scale vs toolkit-only).
6. Prefer Mesa/GBM-friendly drivers; proprietary NVIDIA support is a portability goal if it does not break the atomic/explicit-sync model (final call: QUESTIONS D5).

## Consequences

- New client bindings must expose buffer + sync APIs (XCB-style codegen).
- Compositor role becomes core — WM interaction model needs a clear ADR later.
- Performance wins depend on zero-copy modifier negotiation end-to-end.

## Alternatives

- Server-side drawing as primary forever — rejected for latency/GPU reality.
- Vulkan-only, drop GL — too harsh for Motif/EFL transition.
- Rely on Wayland as the compositor and only keep X clients via XWayland — rejected (X11R8 non-goal; product identity).

## References

- DRM atomic KMS; dma-buf format modifier exchange docs
- Mesa Vulkan WSI atomic KMS work
- Present / DRI3 lessons from X.Org
