# ADR-0005: Rendering path, compositor, and sync

## Status

Accepted — 2026-07-25 (QUESTIONS D1–D5, A2)

## Context

Modern Linux desktops need client-allocated buffers, atomic KMS, explicit sync, and a compositor-owned scanout path. Stakeholder targets a modern Linux desktop with a mandatory built-in compositor and API-agnostic surfaces.

## Decision

1. **Primary native path:** API-agnostic **dmabuf-backed surfaces** (D1). Clients may produce buffers via Vulkan, EGL/GL, or other producers; the protocol deals in buffers + modifiers + sync, not a single GPU API.
2. A **built-in compositor is mandatory** (D2). External compositors are not a v1 architecture dependency; WMs remain clients above the server.
3. **Explicit sync** is first-class for native surfaces; implicit sync retained for legacy drawing paths as needed for the X11R8 corpus.
4. **TearFree / vsync-correct presentation** is the default where hardware allows.
5. Classic X drawing (Render/GC) remains for legacy clients and tests (Glamor-like acceleration allowed).
6. **HDR + ICC** deferred to **v2** (D3).
7. **Fractional / per-monitor scaling:** toolkits **must opt in** (D4). Server does not silently blur-scale unaware legacy clients as the primary policy.
8. **NVIDIA proprietary paths** are supported in addition to GBM/Mesa-friendly stacks (D5), following XLibre-class pragmatism where it does not break the atomic/explicit-sync model for native surfaces.
9. Primary product environment: **modern Linux desktop** (A2).

## Consequences

- Built-in compositor is part of the server deliverable for G1+ (at least Xvfb-quality composition, then real KMS).
- Protocol extensions expose dmabuf import/export, modifiers, and sync objects via XML/XCB codegen (ADR-0006); native IDL is **X12-SURFACE** (ADR-0010).
- Toolkit work (EFL/Motif/etc.) is outside X12 proper but required for scaling opt-in to matter on the desktop.

## Alternatives

- Vulkan-only native API — rejected (D1).
- External-compositor-first (Picom-class) — rejected (D2).
- Server-side scale of all legacy clients — rejected (D4).
- GBM/open-modules only — rejected (D5).

## References

- `docs/QUESTIONS.md` Answers 2026-07-25
- DRM atomic KMS; dma-buf modifier exchange; Mesa Vulkan WSI
