# x12

Next-stage **X Window System** development — evolving the X11 line (`X11R8` / `XCB` lineage) toward a modern, capability-secure, GPU-native window system without abandoning legacy clients.

## Current phase

Strategy and decision documentation. Implementation lands after stakeholder answers in [`docs/QUESTIONS.md`](docs/QUESTIONS.md).

| Document | Contents |
|---|---|
| [`docs/STRATEGY.md`](docs/STRATEGY.md) | Drawbacks to solve + novel technologies + pillars |
| [`docs/QUESTIONS.md`](docs/QUESTIONS.md) | **Clarifying questions — please answer** |
| [`docs/DECISIONS.md`](docs/DECISIONS.md) | Decision index (Accepted / Provisional) |
| [`docs/LEGACY_TESTS.md`](docs/LEGACY_TESTS.md) | Hard rule: legacy tests must pass through |
| [`docs/adr/`](docs/adr/) | Architecture Decision Records |
| [`tests/legacy/`](tests/legacy/) | Legacy suite contract / harness placeholder |

## Accepted constraints (so far)

1. **Document all decisions** (ADR process).
2. **All legacy tests must pass through** — regressions block merge once the corpus is wired.

## Related repositories

- [`dimmus/X11R8`](https://github.com/dimmus/X11R8) — monolithic X11 baseline
- [`dimmus/XCB`](https://github.com/dimmus/XCB) — preferred client binding style
- Motif / EFL / Enlightenment / Window Maker / CDE — consumer stacks above X

## License

TBD (default assumption: MIT/X11-style continuity with X.Org — confirm in QUESTIONS G2).
