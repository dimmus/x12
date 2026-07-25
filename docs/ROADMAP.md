# X12 Roadmap — toward G1

**Owner:** Dimmus  
**Milestone G1:** Xvfb + xterm + capability deny keylog + one Vulkan client  
**Gate:** X11R8 legacy corpus green ([LEGACY_TESTS.md](LEGACY_TESTS.md))

## Sequence

```text
1. [DONE] Import / vendor X11R8 baseline (meson tree, Xvfb + xterm + legacy suite)
2. [DONE] Stand up CI: meson build + ./tests/legacy/run.sh (fail on corpus regress)
3. [DONE] Hierarchical level assignment (CLI) + XACE mediation (X12-LEVEL)
4. [DONE] Security test: sandbox client cannot keylog (QueryKeymap scrubbed)
5. [DONE] XML extension sketches: dmabuf surface + sync (XCB-style) — `X12-SURFACE`
6. [DONE] Memory-safe protocol front-end spike (Rust / ADR-0011) for X12-SURFACE decode/validate
6.5 [DONE] Prep for step 7: C ABI out-struct, server QueryVersion stub, level bridge, drift gate (ADR-0012)
7. [DONE] Built-in compositor path (Xvfb mmap+CopyArea) + Vulkan lavapipe client (ADR-0013)
8. Tag G1 when demo script + legacy suite are green
```

### Notes

- Provenance: ADR-0008 / `docs/vendor/X11R8-README.md`
- Meson: `meson.options` (Meson ≥ 1.1); needs `cargo` for X12-SURFACE server link
- Legacy harness: `./tests/legacy/run.sh` (+ deny_keylog + QueryVersion + vk_present)
- Levels: `-client-level`, `-sandbox-clients` (see `docs/BUILD.md`); bridged to Rust via `X12LevelToProto`
- Surfaces: `proto/xcb/src/x12_surface.xml`; compositor `server/Xext/x12surface.c` (ADR-0013)
- Safe front-end: `safe/x12-proto` (ADR-0011/0012); `./tests/safe_proto/run.sh` (+ drift check)
- Vulkan demo: `./tests/surface/run_vk_present.sh` (lavapipe + memfd → Present)
- Workaround: Xvfb `-extension XFree86-Bigfont` until QueryFont/Bigfont is fixed


## Explicitly after G1

- Toolkit-opt-in fractional / per-monitor scaling
- Broader KMS desktop bring-up (Intel/AMD + proprietary NVIDIA)
- Fine request-matrix documentation for sandbox/user/full
- HDR/ICC (v2)
- Remoting (only if new ADR reopens E*)

## Non-goals that must not sneak into G1

Wayland compositor identity, XWayland, XWin/XQuartz, full DE, remoting stack, HDR as a blocker.
