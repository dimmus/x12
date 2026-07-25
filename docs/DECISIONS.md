# X12 Decision Log

All architecture and process decisions for `dimmus/x12`.  
Detailed records live in [`adr/`](adr/). Strategy: [`STRATEGY.md`](STRATEGY.md). Answers: [`QUESTIONS.md`](QUESTIONS.md).

## How to add a decision

1. Copy the ADR pattern from `adr/0001-record-architecture-decisions.md`.
2. Add a row below.
3. Status changes that alter Accepted architecture require **Dimmus** acknowledgment (ADR-0007).

---

## Decision index

| ID | Title | Status | Date | Notes |
|---|---|---|---|---|
| [ADR-0001](adr/0001-record-architecture-decisions.md) | Record architecture decisions | **Accepted** | 2026-07-25 | ADR process mandatory |
| [ADR-0002](adr/0002-compatibility-first.md) | Native X11 wire, independent line | **Accepted** | 2026-07-25 | A1=a, A3, B3=a, B4 rebuild, H1–H5 |
| [ADR-0003](adr/0003-capability-security-model.md) | Hierarchical security (sandbox/user/full) | **Accepted** | 2026-07-25 | Default **full**; grants via all channels |
| [ADR-0004](adr/0004-legacy-test-gate.md) | Legacy test gate | **Accepted** | 2026-07-25 | X11R8 corpus (B2) |
| [ADR-0005](adr/0005-rendering-and-sync.md) | dmabuf + built-in compositor | **Accepted** | 2026-07-25 | HDR v2; toolkit opt-in scale; NVIDIA proprietary OK |
| [ADR-0006](adr/0006-protocol-and-implementation.md) | XML/XCB IDL + hybrid C/safe front-end | **Accepted** | 2026-07-25 | Bindings: C/XCB first; MIT/X11 license |
| [ADR-0007](adr/0007-milestone-governance-scope.md) | G1 milestone, owner, deferred remoting | **Accepted** | 2026-07-25 | Owner: Dimmus; E* skipped |
| [ADR-0008](adr/0008-x11r8-baseline-import.md) | Vendor-copy X11R8 baseline | **Accepted** | 2026-07-25 | SHA `9b6e8f9`; ROADMAP step 1 |
| [ADR-0009](adr/0009-meson-options-and-x12level.md) | meson.options + X12-LEVEL | **Accepted** | 2026-07-25 | Meson ≥1.1; deny-keylog G1 |
| [ADR-0010](adr/0010-x12-surface-protocol.md) | X12-SURFACE dmabuf + sync IDL | **Accepted** | 2026-07-25 | ROADMAP step 5; server in step 7 |
| [ADR-0011](adr/0011-rust-protocol-frontend.md) | Rust protocol front-end | **Accepted** | 2026-07-25 | ROADMAP step 6; `safe/x12-proto` |
| [ADR-0012](adr/0012-surface-stub-and-safe-link.md) | Surface stub + linked Rust decode | **Accepted** | 2026-07-25 | Pre–step 7; QueryVersion live |
| [ADR-0013](adr/0013-xvfb-compositor-vulkan.md) | Xvfb compositor + Vulkan client | **Accepted** | 2026-07-25 | ROADMAP step 7; lavapipe/memfd |
| [ADR-0014](adr/0014-g1-trim-legacy-surface.md) | Trim G1 defaults; split X12 smokes | **Accepted** | 2026-07-25 | Keep B2 gate; drop unused defaults |
| [ADR-0015](adr/0015-g1-milestone-tag.md) | Tag milestone G1 | **Accepted** | 2026-07-25 | Tag `g1`; version `12.0.0-g1` |
| [ADR-0016](adr/0016-best-of-best-post-g1.md) | Post-G1 best-of-best slice | **Accepted** | 2026-07-25 | Matrix, DRI3 path, fuzz/ASAN, Xephyr |
| [ADR-0017](adr/0017-x11-compat-test-only.md) | X11 compat/test only | **Accepted** | 2026-07-25 | Native = X12-SURFACE; B2 stays |

---

## Accepted decisions (summary)

| ID | Summary |
|---|---|
| D-ACC-1 | Document all architecture choices as ADRs |
| D-ACC-2 | Legacy X11R8 tests (meson / xauth / x11perf / xcmstest) are a merge gate |
| D-ACC-3 | Product = protocol + server for modern Linux desktop; independent `dimmus/x12` |
| D-ACC-4 | Speak X11 natively; evolve via extensions; rebuild OK for native libs |
| D-ACC-5 | Hierarchical levels sandbox/user/full; default **full**; automation without global disable |
| D-ACC-6 | Grant UX: CLI + polkit-like agent + auth tokens + compositor prompts |
| D-ACC-7 | Multi-user/nested: in scope with most up-to-date seat/session design |
| D-ACC-8 | API-agnostic dmabuf surfaces; mandatory built-in compositor; explicit sync |
| D-ACC-9 | HDR/ICC in v2; fractional scale toolkit-opt-in; support proprietary NVIDIA paths |
| D-ACC-10 | XML (XCB-style) IDL; hybrid memory-safe protocol front-end + C core |
| D-ACC-11 | Client bindings priority: C/XCB |
| D-ACC-12 | G1 = Xvfb + xterm + deny keylog + one Vulkan client |
| D-ACC-13 | License MIT/X11-style; decision owner Dimmus |
| D-ACC-14 | Non-goals: not Wayland compositor; no XWayland/XWin/XQuartz; not a DE; no obscure-extension guarantee |
| D-ACC-15 | Remoting deferred (E skipped) |
| D-ACC-16 | Native surfaces = `X12-SURFACE` XML (dmabuf/fourcc/modifier/syncobj); DRI3/Present kept for legacy |
| D-ACC-17 | Protocol front-end language = **Rust** (`safe/x12-proto`); C ABI for dix hookup |
| D-ACC-18 | X12-SURFACE requests decode-first via linked Rust; QueryVersion live |
| D-ACC-19 | Xvfb Present = mmap LINEAR + CopyArea; Vulkan client via lavapipe→memfd (G1); Syncobj still BadImplementation |
| D-ACC-20 | G1 defaults: Xvfb only (no Xnest/Xephyr); toy demos/fontsrv/intel/Xprint/XEvie off; X12 smokes in `tests/x12/` |
| D-ACC-21 | Milestone G1 tagged `g1`; meson version `12.0.0-g1`; evidence in `docs/G1.md` |
| D-ACC-22 | Post-G1: X12-LEVEL matrix + DRI3 CreateSurface + XML wire codegen/fuzz + ASAN/Xephyr CI |
| D-ACC-23 | X11/XCB classic wire = compat + user-app/B2 tests only; product investment is native X12 (ADR-0017) |

---

## Open implementation spikes (not undecided product strategy)

| Topic | Next artifact |
|---|---|
| Exact hierarchical request matrix | Security extension protocol ADR |
| KMS/GBM dmabuf import (non-Xvfb) | ROADMAP after-G1 item A |
| XML→Rust codec codegen | Follow-up when hand validators grow costly |
| Remoting | Only if Dimmus reopens E* with a new ADR |

---

## Stakeholder answer log

| Date | Source | Effect |
|---|---|---|
| 2026-07-25 | Initial request | ADR-0001, ADR-0004; strategy draft |
| 2026-07-25 | Answers in `QUESTIONS.md` | Promoted ADR-0002/0003/0005; added ADR-0006/0007; locked D-ACC-3…15 |
| 2026-07-25 | Implementation start | ADR-0008; X11R8 tree imported; legacy suite + CI green |
| 2026-07-25 | Meson + levels | ADR-0009; `meson.options`; X12-LEVEL; deny_keylog test |
| 2026-07-25 | Surface IDL | ADR-0010; `x12_surface.xml`; ROADMAP step 5 |
| 2026-07-25 | Safe front-end | ADR-0011; Rust `safe/x12-proto`; ROADMAP step 6 |
| 2026-07-25 | Step 7 prep | ADR-0012; ABI out-struct; X12-SURFACE stub; drift + QueryVersion smoke |
| 2026-07-25 | Compositor + Vulkan | ADR-0013; mmap Present; lavapipe vk_present demo |
| 2026-07-25 | Trim legacy surface | ADR-0014; slim meson defaults; split X12 smokes |
| 2026-07-25 | Tag G1 | ADR-0015; tag `g1`; `docs/G1.md` |
| 2026-07-25 | Best-of-best slice | ADR-0016; matrix; DRI3 path; fuzz/ASAN; Xephyr |
| 2026-07-25 | X11 scope | ADR-0017; X11 = compat/test; native X12 first |
