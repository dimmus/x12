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

---

## Open implementation spikes (not undecided product strategy)

| Topic | Next artifact |
|---|---|
| Memory-safe front-end language (Rust vs Zig vs other) | Follow-up ADR when spike completes |
| Exact hierarchical request matrix | Security extension protocol ADR |
| X11R8 tree import/vendor strategy (submodule vs copy) | Implementation PR + short ADR if needed |
| Remoting | Only if Dimmus reopens E* with a new ADR |

---

## Stakeholder answer log

| Date | Source | Effect |
|---|---|---|
| 2026-07-25 | Initial request | ADR-0001, ADR-0004; strategy draft |
| 2026-07-25 | Answers in `QUESTIONS.md` | Promoted ADR-0002/0003/0005; added ADR-0006/0007; locked D-ACC-3…15 |
| 2026-07-25 | Implementation start | ADR-0008; X11R8 tree imported; legacy suite + CI green |
