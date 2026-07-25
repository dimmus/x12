# X12 Strategy — State of the Art Analysis

**Status:** Accepted baseline (answers locked 2026-07-25)  
**Date:** 2026-07-25  
**Scope:** Independent protocol + server successor (`dimmus/x12`) for the modern Linux desktop  
**Hard constraint:** All designated X11R8 legacy tests must pass through (see [LEGACY_TESTS.md](LEGACY_TESTS.md))  
**Owner:** Dimmus

---

## 1. Positioning

X12 is not “Wayland with X branding.” It is an **independent direct successor** of the X Window System: keep protocol clarity, toolability, and the WM/toolkit ecosystem; replace ambient client trust, server-side drawing as the default path, bolted-on HiDPI, and implicit GPU sync. **Remoting is deferred** (not a v1 pillar). HDR/ICC is **v2**.

Related prior art in this org:

| Project | Role relative to X12 |
|---|---|
| `dimmus/X11R8` | Monolithic, mesonized, embedded-oriented X11 baseline |
| `dimmus/XCB` | Preferred client binding; protocol-first, latency-hiding |
| `dimmus/motif`, `efl`, `enlightenment`, `wmaker`, `cde` | Consumer stack that must keep working |

External landscape (2025–2026):

- **X.Org Server** — maintenance / security releases; limited feature velocity.
- **XLibre** — active X11 fork: TearFree defaults, atomic modesetting, **Xnamespace** isolation via XACE.
- **Wayland** — dominant local desktop compositor model; strong isolation, weak network transparency / global tooling story.
- **Mercurius / MWS drafts** — zero-trust, network-transparent, GPU-accelerated remote window system ideas (IETF-adjacent).
- Community **X12 proposal** (xorg-devel, Mar 2026) — hierarchical permissions + native modern rendering.

---

## 2. Drawbacks that must be solved

These are structural problems in classic X11 / X.Org practice. X12 must address them by design, not by optional patches alone.

### D1 — Ambient trust / no client isolation (Critical)

Any authenticated client can, by default: keylog, inject input, read other windows, steal selections, overlay UI.

**Why it matters:** Desktop threat model is now multi-app sandboxing, browsers, Flatpak/Snap, untrusted remote clients.  
**Direction (locked):** Hierarchical levels `sandbox` / `user` / `full` (default **full**); grants via CLI, polkit-like agent, auth tokens, and compositor prompts; trusted automation without a global security-off switch (ADR-0003).

### D2 — Centralized server drawing path & latency (Critical)

Core protocol still encodes a server that owns much of the rendering story. Even with DRI3/Present/Glamor, the architecture pays for extra round-trips and copies compared with compositor-owned scanout.

**Direction (locked):** API-agnostic dmabuf surfaces; **built-in compositor** mandatory; Present-like feedback via in-place X11 extensions (ADR-0005). Native IDL: **X12-SURFACE** (`proto/xcb/src/x12_surface.xml`, ADR-0010).

### D3 — Implicit synchronization & tear / stutter (High)

Legacy implicit dma-buf fencing fights Vulkan explicit sync and multi-GPU / mixed-refresh setups.

**Direction:** Explicit sync first-class (syncobj / sync_file); TearFree / atomic flips as defaults where hardware allows.

### D4 — HiDPI, fractional & per-monitor scaling as hacks (High)

RandR + toolkit heuristics; inconsistent coordinates; mixed DPI pain.

**Direction (locked):** Per-output / fractional scaling via **toolkit opt-in** (D4). No silent server-side blur-scale of unaware clients as primary policy.

### D5 — HDR / color management not first-class (High)

Color profiles and HDR are fragmented across apps, compositors, and ICC side channels.

**Direction (locked):** HDR + ICC in **v2** (D3). v1 may keep minimal existing color hooks needed for legacy tests only.

### D6 — Input model aged for mouse+keyboard era (Medium–High)

Touch, gestures, tablets, high-rate pointers, and accessibility tools rely on uneven extension stacks (XI2, XKB, synaptics leftovers).

**Direction:** Unified modern input path; libinput-class device model; gesture/tablet as core or versioned core extensions; keep XTest-class APIs behind capability gates (for automation/gaming macros).

### D7 — Network transparency vs security tension (Deferred)

X11’s famous remote display is also a large attack surface and a poor fit for GPU frame transport.

**Direction:** **Deferred** — E* skipped. Local modern Linux desktop is v1 focus (ADR-0007). Revisit only under a new ADR.

### D8 — Extension sprawl & protocol debt (Medium)

Hundreds of extensions with overlapping semantics (DAMAGE/Composite/Render/GLX/DRI2/DRI3…). Hard to implement, hard to secure, hard to test.

**Direction:** Thin **core X12** + versioned optional modules; generate client bindings from a single protocol description (XML/YAML → XCB-like); deprecate overlapping legacy paths behind compatibility shims.

### D9 — Memory safety / CVE churn in C servers (High)

Repeated buffer overflows, UAF, integer issues in XKB, DRI, etc.

**Direction:** Hardened C core short-term; memory-safe components for parsers/auth/protocol decode where practical; fuzzing CI as a release gate; privilege separation of input / auth / rendering.

### D10 — Multi-seat, multi-GPU, mixed refresh (Medium)

Xinerama/RandR era assumptions; NVIDIA/GBM/EGL edge cases; lease/VR display awkwardness.

**Direction:** Atomic KMS, DRM leases, explicit multi-GPU buffer paths, per-CRTC timing as native concepts.

### D11 — Accessibility & global tooling regress under pure sandbox models (Medium)

Wayland’s isolation broke many X tools; X11’s openness is unsafe.

**Direction:** **Consented capability APIs** for assistive tech, global hotkeys, screen readers, automation — not ambient access, not total lockdown.

### D12 — Build / packaging fragmentation (Medium for this lineage)

Historical modular X.Org vs your monolithic `X11R8` approach.

**Direction:** Keep monolithic meson tree for embedded/minimal; modular optional features; one CI that runs the full legacy suite.

---

## 3. Novel technologies to take into account

| Technology | Why X12 should care |
|---|---|
| **DRM Atomic KMS** | Transactional modeset; prerequisite for robust multi-output / HDR / modifiers |
| **dma-buf + DRM format modifiers** | Universal zero-copy buffer interchange across GPU/display/media |
| **Explicit sync (drm_syncobj, sync_file)** | Correct Vulkan / multi-queue / Present semantics without over-sync |
| **Vulkan WSI / VK_EXT_image_drm_format_modifier** | Modern client rendering path; atomic KMS WSI in Mesa 25.3+ |
| **GBM / EGLStream alternatives** | Keep EGL path for legacy GL; prefer Vulkan+dmabuf for new clients |
| **XACE + capability frameworks** | Existing X security hook points; XLibre Xnamespace as a concrete design study |
| **XCB / XML protocol codegen** | Continue `dimmus/XCB` pattern for X12 protocol bindings |
| **PipeWire / portals (capture)** | Interop for screen share without ambient root window access |
| **libinput / libei / libeis** | Modern input + remote/emulated input with consent |
| **HDR / color (KMS HDR, ICC, scRGB/PQ)** | First-class color pipeline |
| **QUIC / SCTP + TLS 1.3** | Remoting deferred (E*); keep as future study only |
| **Seat management (seatd / logind)** | Non-systemd and systemd deployments (XLibre already tracks seatd) |
| **Memory-safe protocol codecs (Rust/Zig optional crates)** | Isolate untrusted byte streams from C renderer core |
| **Fuzzing (libFuzzer/AFL++) + ASAN/UBSAN CI** | Mandatory given Xserver CVE history |
| **Wayland interop (optional bridge)** | Not a replacement goal; optional compatibility for mixed desktops |

---

## 4. Architectural pillars (locked)

See [DECISIONS.md](DECISIONS.md) and [adr/](adr/).

1. **Native X11 wire, extensions in place** — rebuild OK for native libs (ADR-0002).
2. **Hierarchical security** — sandbox / user / full; default **full**; multi-channel grants (ADR-0003).
3. **API-agnostic dmabuf + built-in compositor** — explicit sync; proprietary NVIDIA paths supported (ADR-0005).
4. **XML/XCB IDL + hybrid impl** — Rust protocol front-end (`safe/x12-proto`, ADR-0011), C core (ADR-0006).
5. **Legacy test gate** — X11R8 meson / xauth / x11perf / xcmstest (ADR-0004).
6. **Local desktop first** — remoting deferred (ADR-0007).

---

## 5. What X12 deliberately is *not* (locked)

- A Wayland compositor implementation (H1)
- An XWayland host (H2)
- XWin / XQuartz in v1 (H3)
- A full desktop environment (H4)
- A guarantee of every obscure historical extension (H5)
- A remoting-first product in v1 (E skipped)

---

## 6. Success criteria

### G1 (first public milestone)

- Xvfb path up
- xterm works
- Restricted client **cannot keylog** another client
- One Vulkan client via dmabuf native path
- Designated X11R8 legacy suite green (as imported)

### Later

- Tear-free presentation on Intel/AMD and proprietary NVIDIA configs
- Toolkit-opt-in per-monitor fractional scaling
- HDR/ICC (v2)
- XML protocol generates XCB-style bindings for new extensions

---

## 7. Documents in this set

| Doc | Purpose |
|---|---|
| [QUESTIONS.md](QUESTIONS.md) | Clarifying questions + **Answers 2026-07-25** |
| [LEGACY_TESTS.md](LEGACY_TESTS.md) | Legacy pass-through policy |
| [DECISIONS.md](DECISIONS.md) | Decision index |
| [adr/](adr/) | ADRs 0001–0007 |
