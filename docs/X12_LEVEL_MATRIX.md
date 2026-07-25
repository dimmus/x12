# X12-LEVEL request matrix

**ADR:** [0016](adr/0016-best-of-best-post-g1.md) · [0018](adr/0018-native-sync-compositor-scale.md) · implementation `server/Xext/x12level.c`

**Note:** sandbox ≠ WM. Sandbox is for untrusted clients; a window manager needs `user` or `full`.

| Capability | sandbox | user | full |
|---|---|---|---|
| QueryKeymap / keyboard DixRead | deny | allow | allow |
| GrabKey / keyboard DixGrab | deny | deny | allow |
| XTest / keyboard DixWrite inject | deny | deny | allow |
| Pointer / XI2 device DixRead|Grab|Write | deny | allow† | allow |
| Receive events on foreign windows | deny | allow | allow |
| GetImage / drawable DixRead (foreign or root) | deny | deny | allow |
| Foreign selection access | deny | deny | allow |
| Foreign window property read | deny | allow | allow |
| SendEvent to foreign windows | deny | allow | allow |
| XFixes cursor image DixRead | deny | allow | allow |
| XTEST / RECORD extension use | deny | deny | allow |
| X12-SURFACE Present (Rust gate) | deny | allow | allow |

† user: pointer OK; keyboard grab/inject still denied.

## Tests

```sh
./tests/security/run_deny_keylog.sh   # QueryKeymap scrub
./tests/security/run_deny_matrix.sh   # GetImage + props + XTest + keylog + cursor
```

CLI: `-client-level sandbox|user|full`, `-sandbox-clients N,M`.
