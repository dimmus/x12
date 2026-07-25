# X12-LEVEL request matrix

**ADR:** [0016](adr/0016-best-of-best-post-g1.md) · implementation `server/Xext/x12level.c`

| Capability | sandbox | user | full |
|---|---|---|---|
| QueryKeymap / keyboard DixRead | deny | allow | allow |
| GrabKey / keyboard DixGrab | deny | deny | allow |
| XTest / keyboard DixWrite inject | deny | deny | allow |
| Receive events on foreign windows | deny | allow | allow |
| GetImage / drawable DixRead (foreign or root) | deny | deny | allow |
| Foreign selection access | deny | deny | allow |
| Foreign window property read | deny | allow | allow |
| SendEvent to foreign windows | deny | allow | allow |
| XTEST / RECORD extension use | deny | deny | allow |
| X12-SURFACE Present (Rust gate) | deny | allow | allow |

## Tests

```sh
./tests/security/run_deny_keylog.sh   # QueryKeymap scrub
./tests/security/run_deny_matrix.sh   # GetImage + props + XTest + keylog
```

CLI: `-client-level sandbox|user|full`, `-sandbox-clients N,M`.
