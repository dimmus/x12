# Research notes — display stack landscape (2026-07)

Working notes that feed [`STRATEGY.md`](../STRATEGY.md). Not normative; ADRs are normative.

## X11 liabilities (consensus)

- Ambient client trust → keylog / inject / capture
- Extension sprawl and CVE surface in C servers
- HiDPI / fractional scale / HDR bolted on
- Implicit GPU sync vs Vulkan explicit sync
- Network transparency valuable but unsafe and inefficient for GPU frames

## Active X lines

| Line | Observation for X12 |
|---|---|
| X.Org Server | Security/maintenance focus; limited feature velocity |
| XLibre | TearFree defaults, atomic modesetting, **Xnamespace** via XACE — strongest live X11 security experiment |
| X11R8 (dimmus) | Monolithic meson, embedded, integrated XCB/drivers/apps; drops XWayland/XWin/XQuartz |
| Community X12 mail (2026-03) | Hierarchical permissions + native modern rendering — aligned directionally |

## Adjacent designs

- **Wayland:** local compositor security/performance model; weak network/global-tool story
- **Mercurius/MWS drafts:** zero-trust network-transparent GPU workstation — remoting inspiration, not a drop-in
- **Portals + PipeWire + libei:** consent-based capture and input emulation patterns to reuse conceptually

## Graphics primitives to assume

- Atomic KMS
- dma-buf + format modifiers
- Explicit sync (syncobj / sync_file)
- Vulkan WSI with DRM modifiers; EGL/GBM retained for legacy GL
- Per-CRTC timing / mixed refresh as normal

## Implication

X12’s differentiation vs Wayland should be: **capability-scoped openness**, **tooling/WM ecosystem continuity**, and a **first-class remoting profile** — while matching Wayland-class buffer/sync/security hygiene on the local path.
