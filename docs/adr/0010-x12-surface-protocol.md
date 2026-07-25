# ADR-0010: X12-SURFACE protocol (dmabuf + explicit sync)

## Status

Accepted — 2026-07-25 (ROADMAP step 5; ADR-0005 / ADR-0006)

## Context

G1 needs a native path for client-allocated GPU buffers into the built-in compositor without tying new clients to DRI3/GLX. ADR-0005 requires API-agnostic dmabuf surfaces and first-class explicit sync. ADR-0006 requires XCB-style XML as the IDL.

## Decision

1. Add extension **`X12-SURFACE`** (`proto/xcb/src/x12_surface.xml`, header `x12_surface`).
2. Wire deals in **dmabuf plane FDs**, **DRM fourcc formats**, **modifiers**, and **timeline syncobjs** — not Vulkan/EGL specifically.
3. First-class XIDs: `SURFACE`, `SYNCOBJ`, `EVENT` (local to this extension; no DRI3 dependency for new clients).
4. Core requests: `QueryVersion`, `QueryCapabilities`, `QueryModifiers`, `CreateSurface`, `DestroySurface`, `ImportSyncobj`, `FreeSyncobj`, `Attach`, `Detach`, `Present`, `SelectInput`.
5. Feedback via XGE: `ConfigureNotify`, `CompleteNotify`, `IdleNotify`.
6. Server/compositor implementation and Vulkan sample client remain **ROADMAP step 7**; this ADR lands the IDL sketch and codegen wiring only.
7. Legacy DRI3/Present remain for existing clients and the X11R8 corpus.

## Consequences

- C/XCB bindings can be generated from the XML for step 6–7 work.
- Built-in compositor must eventually implement these opcodes; until then QueryVersion may be absent on the wire.
- Modifier/format negotiation is drawable-scoped; device-open paths stay out of this extension (clients allocate via their GPU API).

## Alternatives

- Reuse DRI3 pixmap + Present only — rejected: forces PIXMAP/DRI3 for native Vulkan path (conflicts with D1 API-agnostic surfaces).
- Wayland linux-dmabuf clone as sole IDL — rejected: product speaks X11 wire with in-place extensions (ADR-0002).

## References

- ADR-0005, ADR-0006, ADR-0007
- `docs/X12_SURFACE.md`
- `proto/xcb/src/dri3.xml`, `proto/xcb/src/present.xml`
