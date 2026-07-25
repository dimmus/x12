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

## G1 milestone

**Xvfb + xterm + capability deny keylog + one Vulkan client**

Progress: baseline builds + legacy gate + **X12-LEVEL** hierarchical security (deny-keylog test green). Remaining: dmabuf/Vulkan client path.

Build uses **`meson.options`** (Meson ≥ 1.1).

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
| [`docs/ROADMAP.md`](docs/ROADMAP.md) | Path to G1 |
| [`docs/DECISIONS.md`](docs/DECISIONS.md) | ADR index |
| [`docs/LEGACY_TESTS.md`](docs/LEGACY_TESTS.md) | Legacy pass-through policy |
| [`docs/QUESTIONS.md`](docs/QUESTIONS.md) | Stakeholder answers |
| [`docs/adr/`](docs/adr/) | Architecture Decision Records |
