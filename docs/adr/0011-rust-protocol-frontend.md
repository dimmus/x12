# ADR-0011: Rust for the memory-safe protocol front-end

## Status

Accepted — 2026-07-25 (ROADMAP step 6; ADR-0006 spike)

## Context

ADR-0006 requires a hybrid split: a memory-safe front-end for parse/validate/auth
of untrusted protocol bytes, and a C core for resources/compositor/DRM. The
language choice was deferred to this spike. Zig is not available in the current
dev/CI environment; Rust 1.83 is.

## Decision

1. **Language:** **Rust** for the protocol front-end crate (`safe/x12-proto`).
2. **First codec target:** **X12-SURFACE** request decode + validation (ADR-0010),
   including FD-count checks and hierarchical level gating (ADR-0003).
3. **Integration surface:** stable **C ABI** (`x12_proto_decode_surface`) for a
   future dix hook; the X server request loop is **not** switched in this spike.
4. **Build:** Cargo for the crate; Meson/CI invoke `cargo test` via
   `./tests/safe_proto/run.sh`. Optional meson option `build-safe-proto`.
5. **XML codegen into Rust** remains future work; hand-written validators are
   acceptable for the G1 spike and must stay aligned with
   `proto/xcb/src/x12_surface.xml`.

## Consequences

- CI images need a Rust toolchain (`rustc`/`cargo`).
- Fuzzing (libFuzzer/cargo-fuzz) targets this crate first.
- Zig (or another language) would require a new ADR if revisited.

## Alternatives

- **Zig** — deferred; toolchain not present; revisit if packaging prefers it.
- **Safe C subset / bounded C** — rejected for the untrusted decode path (F2).
- **Rewrite dix in Rust** — out of scope for G1 (ADR-0006).

## References

- ADR-0003, ADR-0006, ADR-0010
- `safe/x12-proto/`, `docs/SAFE_PROTO.md`
