# X12-native G1 smokes

Complementary to the locked X11R8 corpus in [`../legacy/`](../legacy/).

```sh
./tests/x12/run.sh
```

| Smoke | Script |
|---|---|
| Sandbox deny keylog | `tests/security/run_deny_keylog.sh` |
| Request matrix | `tests/security/run_deny_matrix.sh` |
| X12-SURFACE QueryVersion | `tests/surface/run_query_version.sh` |
| Vulkan → Present (lavapipe) | `tests/surface/run_vk_present.sh` |
| Xephyr nested (opt-in build) | `tests/x12/run_xephyr.sh` |

CI: `./tests/legacy/run.sh` after B2; plus `asan-x12` and `xephyr-smoke` jobs (ADR-0016).
