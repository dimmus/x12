# X12 Decision Log

All architecture and process decisions for `dimmus/x12`.  
Detailed records live in [`adr/`](adr/). Strategy analysis: [`STRATEGY.md`](STRATEGY.md). Open questions: [`QUESTIONS.md`](QUESTIONS.md).

## How to add a decision

1. Copy the ADR pattern from `adr/0001-record-architecture-decisions.md`.
2. Add a row below.
3. Link related QUESTION ids when Provisional.
4. On stakeholder answer, set Status → Accepted and note date + summary.

---

## Decision index

| ID | Title | Status | Date | Notes |
|---|---|---|---|---|
| [ADR-0001](adr/0001-record-architecture-decisions.md) | Record architecture decisions | **Accepted** | 2026-07-25 | ADR process mandatory |
| [ADR-0002](adr/0002-compatibility-first.md) | Compatibility-first evolution | Provisional | 2026-07-25 | Awaiting A1, B3, B4 |
| [ADR-0003](adr/0003-capability-security-model.md) | Capability / hierarchical security | Provisional | 2026-07-25 | Awaiting C1–C5 |
| [ADR-0004](adr/0004-legacy-test-gate.md) | Legacy test gate | **Accepted** | 2026-07-25 | Stakeholder hard constraint |
| [ADR-0005](adr/0005-rendering-and-sync.md) | Rendering path and sync | Provisional | 2026-07-25 | Awaiting D1–D5 |

---

## Accepted decisions (summary)

### D-ACC-1 — Document everything (ADR-0001)

Architecture choices are written as ADRs in-tree; chat is not authoritative.

### D-ACC-2 — Legacy tests are a merge gate (ADR-0004)

All designated legacy tests must pass. Removals need ADR + explicit approval. Policy text: [`LEGACY_TESTS.md`](LEGACY_TESTS.md).

### D-ACC-3 — Analysis baseline frozen for review (2026-07-25)

[`STRATEGY.md`](STRATEGY.md) captures drawbacks D1–D12 and the novel-tech inventory as the working baseline until superseded by a later ADR.

---

## Provisional decisions (summary)

| ID | Summary | Blocks on |
|---|---|---|
| D-PROV-1 | Evolve X; do not replace with Wayland identity | A1, A3, H1–H3 |
| D-PROV-2 | Dual engine: legacy X11 path + native X12 surfaces | B3, B4 |
| D-PROV-3 | Hybrid namespaces + capabilities; default deny sensitive ops | C1–C4 |
| D-PROV-4 | dmabuf + explicit sync + atomic KMS as native path | D1–D5 |
| D-PROV-5 | Remoting is a designed transport profile, not raw ambient X | E1–E3 |
| D-PROV-6 | Continue XCB-style protocol codegen | F1, F3 |
| D-PROV-7 | Monolithic meson-friendly tree (X11R8 lineage) for embedded/minimal | A2, D12 |

---

## Explicit non-decisions (open)

| Topic | Why open |
|---|---|
| Wire major version number (stay 11+extensions vs new major 12) | Needs B3/B4 + ecosystem cost analysis |
| Memory-safe language boundary | Needs F2 spike |
| Built-in vs external compositor | Needs D2 + WM roadmap |
| Mercurius-like remoting vs TLS-wrapped X | Needs E2 spike |
| Relationship to XLibre code | Needs A3 (technical reuse vs independent) |

---

## Stakeholder answer log

| Date | Source | Effect |
|---|---|---|
| 2026-07-25 | Initial request | Accepted ADR-0004 (legacy pass-through); commissioned strategy + questions |

*(Append rows when QUESTIONS.md is answered.)*
