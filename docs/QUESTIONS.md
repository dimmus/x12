# Strategy Clarifying Questions

Please answer these so we can lock ADRs. Reply inline (Y/N, choice, or short prose).  
Unanswered items stay **Provisional** in [DECISIONS.md](DECISIONS.md).

---

## A. Mission & product shape

**A1.** Is X12 meant to be:
- (a) a **protocol + server** successor that other WMs/toolkits adopt, or
- (b) a **full stack** with a reference compositor/WM (EFL/Enlightenment-first), or
- (c) both, with (a) as the public contract?

**A2.** Primary target environments (rank or select):
- Embedded / kiosk (continuing X11R8)
- Classic Unix desktop (twm/wmaker/Motif/CDE)
- Modern Linux desktop competing with Wayland sessions
- Multi-user thin-client / remote GPU workstation
- BSD / non-Linux ports in v1?

**A3.** Relationship to upstream:
- Independent `dimmus/x12` line only?
- Track / vendor patches from X.Org?
- Track / learn from XLibre (Xnamespace, TearFree) without merging politics?
- Attempt eventual upstream contribution?

---

## B. Compatibility & legacy

**B1.** Confirm hard rule: **all legacy tests must pass** on every merge — yes/no?  
Any allowed exceptions (e.g. XWayland-only, XWin, XQuartz — already dropped in X11R8)?

**B2.** Which legacy corpora are in-scope for “must pass”?
- X11R8 meson test suite / xauth scripts / x11perf / xcmstest
- XCB unit/integration tests
- Selected Motif / EFL / Window Maker smoke tests
- XTest / RENDER / GLX conformance subsets
- Other: ___

**B3.** Compatibility strategy preference:
- (a) X12 server speaks X11 protocol natively (extensions evolve in place)
- (b) X11 compatibility shim / translator beside a new native protocol
- (c) Dual-protocol sockets (X11 + X12) in one server process

**B4.** Must existing **binary** Xlib/XCB clients work unchanged (same wire major = 11 with extensions), or is a **rebuild** against new libs acceptable for “X12 native”?

---

## C. Security model

**C1.** Permission model preference:
- Hierarchical levels (sandbox / user / full) as in the Mar 2026 xorg-devel X12 note
- Fine-grained capabilities (capture, inject, foreign-window, selection, global-bind)
- Namespace isolation (XLibre Xnamespace-like) as the primary primitive
- Combination: ___

**C2.** Default for newly connected local clients: sandbox, user, or full?  
**C3.** How should grants be UX’d: CLI flags, polkit-like agent, `.Xauthority` tokens, compositor prompt, all of the above?

**C4.** Must trusted automation (macros, xdotool-class tools, accessibility) remain possible without disabling security globally?

**C5.** Multi-user on one seat / nested servers: in scope for v1?

---

## D. Rendering & graphics

**D1.** Preferred native client GPU API for new apps: Vulkan-first, GL/EGL-first, or API-agnostic dmabuf surfaces?

**D2.** Is a **built-in compositor** mandatory, or must external compositors (Compton/Picom-class, EFL compositor) remain first-class?

**D3.** HDR + ICC: v1 requirement or v2?

**D4.** Fractional per-monitor scaling: must toolkits opt in, or does the server scale legacy clients (with blur tradeoffs)?

**D5.** NVIDIA: require GBM-friendly / open modules only, or also support proprietary paths like XLibre?

---

## E. Network transparency

**E1.** Is remoting a **v1 pillar** or deferred?
**E2.** If remoting: prefer
- (a) modernized X wire over TLS,
- (b) separate frame/input remoting profile (VNC/RDP/PipeWire-like),
- (c) Mercurius-style command + GPU isolation streams,
- (d) undecided — need spike.

**E3.** Must SSH `X11Forwarding`-style workflows keep working for legacy clients?

---

## F. Protocol & implementation language

**F1.** Single source-of-truth protocol IDL: XML (XCB-style), or new YAML/Cap’n Proto/Protobuf-ish schema?

**F2.** Server implementation plan:
- Evolve from X11R8 C server
- Greenfield C
- Hybrid C core + memory-safe protocol front-end
- Other: ___

**F3.** Client bindings priority order: C/XCB, EFL, Motif, Python, Rust, other?

---

## G. Process & milestones

**G1.** First public milestone should demonstrate what? (e.g. “Xvfb + xterm + capability deny keylog + one Vulkan client”)

**G2.** License continuity: MIT/X11-style same as X.Org, or any change?

**G3.** Who are the named decision owners for ADR acceptance?

---

## H. Explicit non-goals (confirm)

Reply keep / drop / rewrite for each provisional non-goal:

- H1. Not a Wayland compositor implementation
- H2. No XWayland (matching X11R8)
- H3. No XWin / XQuartz in v1
- H4. Not a full desktop environment
- H5. No guarantee of wire-compatibility with every obscure historical extension

---

## How to answer

Open an issue comment, PR review, or edit this file with a section:

```md
## Answers (YYYY-MM-DD)

A1: ...
A2: ...
...
```

Then we will promote Provisional ADRs to Accepted.