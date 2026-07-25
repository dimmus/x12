# ADR-0012: X12-SURFACE server stub + linked Rust decode path

## Status

Accepted — 2026-07-25 (pre–step 7 prep)

## Context

Step 6 left a Rust decoder with a status-only C ABI and no server extension.
Step 7 (compositor + Vulkan) needs a hookable decode path, a real
`QueryExtension` target, level policy shared with X12-LEVEL, and a drift gate
so XML / XCB / Rust cannot diverge silently.

## Decision

1. **Expand C ABI** (`x12_proto_surface_decoded_t`) so dix receives validated
   fields for every X12-SURFACE opcode.
2. **Meson builds** `libx12_proto.a` from `safe/x12-proto` and links it into
   servers via `dep_common` when `build-safe-proto` + `cargo` are available
   (`X12SURFACE` in `dix-config`).
3. **Server stub** `server/Xext/x12surface.c`:
   - Every request runs `x12_proto_decode_surface` first with
     `X12LevelToProto(X12LevelOfClient(client))`.
   - `QueryVersion` replies `1.0`.
   - Other validated opcodes return `BadImplementation` until compositor lands.
4. **Level bridge:** `X12_LEVEL_*` == `X12_PROTO_LEVEL_*` == Rust `ClientLevel`.
5. **Anti-drift:** `tests/safe_proto/drift_check.c` asserts XCB `sizeof` equals
   `X12_PROTO_SZ_*`; CI runs it with `./tests/safe_proto/run.sh`.
6. **Smoke:** `tests/surface/run_query_version.sh` in the legacy suite.

## Consequences

- Main CI needs a Rust toolchain to build Xvfb with X12-SURFACE.
- Without `cargo`, configure warns and disables `X12SURFACE` (servers build,
  extension absent).
- Step 7 replaces `BadImplementation` arms with resource/compositor logic;
  must keep the decode-first gate.

## Alternatives

- C-only stub without Rust — rejected (undermines ADR-0006/0011).
- Delay server registration until compositor — rejected (blocks client bring-up).

## References

- ADR-0003, ADR-0010, ADR-0011
- `docs/SAFE_PROTO.md`, `docs/X12_SURFACE.md`
