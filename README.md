# x12

Independent next-stage **X Window System** — protocol + server for the modern Linux desktop.

Baseline: vendored from [`dimmus/X11R8`](https://github.com/dimmus/X11R8) @ `9b6e8f9` (see [ADR-0008](docs/adr/0008-x11r8-baseline-import.md)).

**Decision owner:** Dimmus  
**License:** MIT/X11-style ([`LICENSE`](LICENSE))

## Quick start

```sh
# deps: see docs/BUILD.md
meson setup build
meson compile -C build
./tests/legacy/run.sh          # must pass
```

## G1 milestone — tagged

**Xvfb + xterm + capability deny keylog + one Vulkan client**

**Status:** achieved — git tag `g1` ([docs/G1.md](docs/G1.md), ADR-0015).  
Post-G1 hardening: request matrix, DRI3 CreateSurface, fuzz/ASAN, Xephyr smoke (ADR-0016).  
Verify: `./tests/legacy/run.sh` (B2 + X12 smokes). Defaults: Xvfb-focused (ADR-0014).

Build uses **`meson.options`** (Meson ≥ 1.1). Version: `12.0.0-g1`.

## Locked strategy

| Pillar | Choice |
|---|---|
| Product | Protocol + server, modern Linux desktop, independent line |
| Wire | Native X11 + extensions in place; rebuild OK |
| Security | Hierarchical `sandbox` / `user` / `full` (default **full**) |
| Graphics | API-agnostic dmabuf; built-in compositor; HDR v2 |
| Legacy gate | X11R8 meson / xauth / x11perf / xcmstest |

## Docs

| Document | Contents |
|---|---|
| [`docs/BUILD.md`](docs/BUILD.md) | Build & run instructions |
| [`docs/STRATEGY.md`](docs/STRATEGY.md) | Drawbacks, novel tech, pillars |
| [`docs/G1.md`](docs/G1.md) | G1 evidence checklist |
| [`docs/ROADMAP.md`](docs/ROADMAP.md) | G1 complete + post-G1 sequence |
| [`docs/DECISIONS.md`](docs/DECISIONS.md) | ADR index |
| [`docs/LEGACY_TESTS.md`](docs/LEGACY_TESTS.md) | Legacy pass-through policy |
| [`docs/QUESTIONS.md`](docs/QUESTIONS.md) | Stakeholder answers |
| [`docs/adr/`](docs/adr/) | Architecture Decision Records |
