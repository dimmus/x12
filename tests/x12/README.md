# X12-native G1 smokes

Complementary to the locked X11R8 corpus in [`../legacy/`](../legacy/).

```sh
./tests/x12/run.sh
```

| Smoke | Script |
|---|---|
| Sandbox deny keylog | `tests/security/run_deny_keylog.sh` |
| X12-SURFACE QueryVersion | `tests/surface/run_query_version.sh` |
| Vulkan → Present (lavapipe) | `tests/surface/run_vk_present.sh` |

CI runs these via `./tests/legacy/run.sh` after the B2 corpus (ADR-0014).
