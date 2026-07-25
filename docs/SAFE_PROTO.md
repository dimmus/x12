# Memory-safe protocol front-end

**Crate:** [`safe/x12-proto`](../safe/x12-proto/)  
**ADRs:** [ADR-0011](adr/0011-rust-protocol-frontend.md), [ADR-0012](adr/0012-surface-stub-and-safe-link.md)  
**IDL source of truth:** [`proto/xcb/src/x12_surface.xml`](../proto/xcb/src/x12_surface.xml)

## Role

```text
client bytes (+ SCM_RIGHTS FDs)
        │
        ▼
 ┌──────────────────────────┐
 │  Rust x12-proto          │  framing · opcode decode · field bounds
 │  + ClientLevel gate      │  FD count checks · sandbox deny
 └────────────┬─────────────┘
              │ x12_proto_surface_decoded_t (C ABI)
              ▼
 ┌──────────────────────────┐
 │  C dix X12-SURFACE stub  │  QueryVersion live; other ops → BadImplementation
 │  (step 7: compositor)    │
 └──────────────────────────┘
```

## Level bridge

| C `X12_LEVEL_*` | Rust / `X12_PROTO_LEVEL_*` |
|---|---|
| 0 sandbox | 0 |
| 1 user | 1 |
| 2 full | 2 |

`X12LevelToProto(X12LevelOfClient(client))` is passed into every decode.

## Level policy (surface opcodes)

| Level | X12-SURFACE |
|---|---|
| sandbox | `QueryVersion`, `QueryCapabilities`, `QueryModifiers` only |
| user / full | all opcodes (unimplemented → `BadImplementation` after decode) |

## Test

```bash
./tests/safe_proto/run.sh          # cargo test + sizeof drift gate
./tests/surface/run_query_version.sh
# meson: meson test -C build --suite safe
```

## C ABI

See [`safe/x12-proto/include/x12_proto.h`](../safe/x12-proto/include/x12_proto.h).
