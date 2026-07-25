# ADR-0006: Protocol IDL and implementation split

## Status

Accepted — 2026-07-25 (QUESTIONS F1–F3, B3, G2)

## Context

`dimmus/XCB` already uses XML protocol descriptions and codegen. Stakeholder wants that continuity, plus a hybrid memory-safe front-end to reduce CVE-class protocol bugs, while keeping a C core for rendering/DRM.

## Decision

1. **Single source-of-truth IDL:** **XML in XCB style** (F1). Extensions (surfaces, hierarchy/security, Present-like feedback) are described there and generate C/XCB bindings.
2. **Implementation split (F2):**
   - **Memory-safe protocol front-end** — parse/validate/auth/decode untrusted byte streams; enforce hierarchical levels.
   - **C core** — resource database, built-in compositor, KMS/DRM, Glamor-like paths, drivers.
3. **Client binding priority:** **C/XCB first** (F3). Other languages later.
4. Wire evolution: native X11 protocol + in-place extensions (ADR-0002); rebuilt libs pick up new extension requests.
5. **License:** MIT/X11-style continuity with X.Org (G2). Add `LICENSE` at implementation import time if not already present.

## Consequences

- Language choice for the memory-safe front-end (Rust/Zig/other) is an implementation spike, not a product identity choice — record the pick in a follow-up ADR when code lands.
- Codegen pipeline is an early milestone dependency for the Vulkan/dmabuf client in G1.
- Fuzzing targets the memory-safe front-end first.

## Alternatives

- Greenfield YAML/Cap’n Proto IDL — rejected (F1).
- Pure C server end-to-end — rejected (F2).
- Pure memory-safe rewrite of the entire server — deferred; too large for G1.

## References

- `dimmus/XCB`
- `docs/QUESTIONS.md` Answers 2026-07-25
