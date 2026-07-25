# x12

Independent next-stage **X Window System** — protocol + server for the modern Linux desktop. Evolves the X11 wire in place (extensions), with hierarchical client security, a built-in compositor, and API-agnostic dmabuf surfaces.

**Decision owner:** Dimmus  
**License:** MIT/X11-style (see [`LICENSE`](LICENSE))

## Locked strategy (2026-07-25)

| Pillar | Choice |
|---|---|
| Product | Protocol + server (not a DE) |
| Wire | Native X11 + extensions in place; rebuild OK for native libs |
| Security | Hierarchical `sandbox` / `user` / `full` (default **full**) |
| Graphics | API-agnostic dmabuf; built-in compositor; NVIDIA proprietary paths OK |
| HDR | v2 |
| Scaling | Toolkit opt-in |
| IDL / impl | XML (XCB-style); hybrid memory-safe front-end + C core |
| Bindings | C/XCB first |
| Remoting | Deferred |
| Legacy gate | X11R8 meson / xauth / x11perf / xcmstest must pass |

## G1 milestone

**Xvfb + xterm + capability deny keylog + one Vulkan client**

## Docs

| Document | Contents |
|---|---|
| [`docs/STRATEGY.md`](docs/STRATEGY.md) | Drawbacks, novel tech, locked pillars |
| [`docs/QUESTIONS.md`](docs/QUESTIONS.md) | Questions + stakeholder answers |
| [`docs/DECISIONS.md`](docs/DECISIONS.md) | Decision index (ADR-0001…0007) |
| [`docs/LEGACY_TESTS.md`](docs/LEGACY_TESTS.md) | Legacy pass-through policy |
| [`docs/ROADMAP.md`](docs/ROADMAP.md) | Engineering sequence toward G1 |
| [`docs/adr/`](docs/adr/) | Architecture Decision Records |
| [`tests/legacy/`](tests/legacy/) | Legacy suite harness contract |

## Related repositories

- [`dimmus/X11R8`](https://github.com/dimmus/X11R8) — legacy corpus + baseline lineage
- [`dimmus/XCB`](https://github.com/dimmus/XCB) — XML/XCB binding style to continue
