# X12 Strategy — State of the Art Analysis

**Status:** Draft for stakeholder review  
**Date:** 2026-07-25  
**Scope:** Next-stage X Window System (`dimmus/x12`), continuing the `X11R8` / `XCB` lineage  
**Hard constraint:** All legacy tests must pass through (see [LEGACY_TESTS.md](LEGACY_TESTS.md))

---

## 1. Positioning

X12 is not “Wayland with X branding.” It is the **direct successor line of the X Window System**: keep what made X valuable (protocol clarity, network-aware design, toolability, WM ecosystem, remote display), and replace the parts that are now architectural liabilities (ambient client trust, server-side drawing as the default path, bolted-on HiDPI/HDR, implicit GPU sync).

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
**Direction:** Capability-based / hierarchical permissions; namespace isolation (learn from XACE + XLibre Xnamespace); explicit grants for global grab, screen capture, synthetic input.

### D2 — Centralized server drawing path & latency (Critical)

Core protocol still encodes a server that owns much of the rendering story. Even with DRI3/Present/Glamor, the architecture pays for extra round-trips and copies compared with compositor-owned scanout.

**Direction:** Client-side buffer ownership as the primary path; compositor/server composites; Present-like feedback as core semantics.

### D3 — Implicit synchronization & tear / stutter (High)

Legacy implicit dma-buf fencing fights Vulkan explicit sync and multi-GPU / mixed-refresh setups.

**Direction:** Explicit sync first-class (syncobj / sync_file); TearFree / atomic flips as defaults where hardware allows.

### D4 — HiDPI, fractional & per-monitor scaling as hacks (High)

RandR + toolkit heuristics; inconsistent coordinates; mixed DPI pain.

**Direction:** Native logical/physical coordinate model; per-output scale factors (including fractional); protocol-level scale negotiation.

### D5 — HDR / color management not first-class (High)

Color profiles and HDR are fragmented across apps, compositors, and ICC side channels.

**Direction:** Protocol-level colorspace / HDR metadata on surfaces and outputs; cooperate with colord / VK_EXT / KMS HDR properties.

### D6 — Input model aged for mouse+keyboard era (Medium–High)

Touch, gestures, tablets, high-rate pointers, and accessibility tools rely on uneven extension stacks (XI2, XKB, synaptics leftovers).

**Direction:** Unified modern input path; libinput-class device model; gesture/tablet as core or versioned core extensions; keep XTest-class APIs behind capability gates (for automation/gaming macros).

### D7 — Network transparency vs security tension (High)

X11’s famous remote display is also a large attack surface and a poor fit for GPU frame transport.

**Direction:** Preserve *session remoting* as a first-class goal, but replace raw untrusted X protocol over the wire with authenticated, capability-scoped, buffer-aware remoting (TLS + stream mux; optionally learn from Mercurius/MWS). Local clients keep a fast Unix path.

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
| **QUIC / SCTP + TLS 1.3** | Candidate transports for secure remoting (study Mercurius) |
| **Seat management (seatd / logind)** | Non-systemd and systemd deployments (XLibre already tracks seatd) |
| **Memory-safe protocol codecs (Rust/Zig optional crates)** | Isolate untrusted byte streams from C renderer core |
| **Fuzzing (libFuzzer/AFL++) + ASAN/UBSAN CI** | Mandatory given Xserver CVE history |
| **Wayland interop (optional bridge)** | Not a replacement goal; optional compatibility for mixed desktops |

---

## 4. Proposed architectural pillars (provisional)

Pending answers in [QUESTIONS.md](QUESTIONS.md). Provisional decisions are recorded in [adr/](adr/).

1. **Compatibility-first evolution** — X11 clients keep working via a compatibility layer; native X12 clients get new buffer + capability APIs.
2. **Capabilities over ambient trust** — default deny for global input, capture, foreign-window ops; user/agent grant paths; privileged “root namespace” for trusted tooling.
3. **Buffers in, drawing optional** — primary path is client-allocated dmabuf/Vulkan/GL surfaces; classic drawing retained for legacy.
4. **Compositor is not optional** — display composition is a core responsibility (can still host external WMs via defined roles).
5. **Network is a transport profile** — local Unix socket fast path; remote profile is authenticated + capability-scoped + optionally compressed/video-encoded for frames.
6. **Legacy test gate** — no merge that regresses the inherited X11R8 / XCB / app test corpus (see policy).

---

## 5. What X12 deliberately should *not* become (provisional)

- A pure Wayland clone that abandons network sessions and global tooling.
- A break-the-world protocol rewrite with no X11 compatibility.
- A politics-driven fork race; technical merit and test gates decide acceptance.
- A kitchen-sink desktop environment — X12 is the window system; EFL/Motif/WMs remain above it.

---

## 6. Success criteria (draft)

- Legacy suite green on every PR (Xvfb / Xephyr / protocol / xauth / selected xapps).
- New capability model demonstrably blocks ambient keylogging without grants.
- Native path: tear-free presentation on at least one Intel/AMD and one NVIDIA configuration.
- Per-monitor fractional scaling works for a reference toolkit (EFL and/or Motif path).
- Documented remoting profile that authenticates and scopes clients.
- Protocol description generates XCB-style bindings.

---

## 7. Documents in this set

| Doc | Purpose |
|---|---|
| [QUESTIONS.md](QUESTIONS.md) | Strategy clarifications — **please answer** |
| [LEGACY_TESTS.md](LEGACY_TESTS.md) | Legacy pass-through policy |
| [adr/0001-record-architecture-decisions.md](adr/0001-record-architecture-decisions.md) | ADR process |
| [adr/0002-compatibility-first.md](adr/0002-compatibility-first.md) | Compatibility stance |
| [adr/0003-capability-security-model.md](adr/0003-capability-security-model.md) | Security model |
| [adr/0004-legacy-test-gate.md](adr/0004-legacy-test-gate.md) | CI gate decision |
| [adr/0005-rendering-and-sync.md](adr/0005-rendering-and-sync.md) | Rendering stack |
| [DECISIONS.md](DECISIONS.md) | Index of all decisions + open items |
