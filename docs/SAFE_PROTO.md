# Memory-safe protocol front-end

**Crate:** [`safe/x12-proto`](../safe/x12-proto/)  
**ADR:** [ADR-0011](adr/0011-rust-protocol-frontend.md)  
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
              │ validated struct / C ABI status
              ▼
 ┌──────────────────────────┐
 │  C dix / compositor core │  (hookup = ROADMAP step 7+)
 └──────────────────────────┘
```

## Level policy (spike)

| Level | X12-SURFACE |
|---|---|
| sandbox | `QueryVersion`, `QueryCapabilities`, `QueryModifiers` only |
| user / full | all opcodes |

Refine with the full hierarchical request matrix after G1.

## Test

```bash
./tests/safe_proto/run.sh
# meson: meson test -C build safe-proto  (when -Dbuild-safe-proto=true)
```

## C ABI

See [`safe/x12-proto/include/x12_proto.h`](../safe/x12-proto/include/x12_proto.h).
