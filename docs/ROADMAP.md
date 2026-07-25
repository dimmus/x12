# X12 Roadmap — G1 complete

**Owner:** Dimmus  
**Milestone G1:** Xvfb + xterm + capability deny keylog + one Vulkan client — **tagged `g1`**  
**Evidence:** [G1.md](G1.md) · [ADR-0015](adr/0015-g1-milestone-tag.md)  
**App/compat gate:** X11R8 corpus green ([LEGACY_TESTS.md](LEGACY_TESTS.md)) — X11 is test/compat only (ADR-0017)

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
8. [DONE] Tag G1 — demo + legacy suite green (tag `g1`, ADR-0015)
```

### Notes

- Provenance: ADR-0008 / `docs/vendor/X11R8-README.md`
- Meson: `meson.options` (Meson ≥ 1.1); needs `cargo` for X12-SURFACE server link
- Legacy harness: `./tests/legacy/run.sh` (B2 corpus → `./tests/x12/run.sh`)
- G1 defaults slimmed: Xvfb only; toy demos / nested DDX off (ADR-0014)
- Levels: `-client-level`, `-sandbox-clients` (see `docs/BUILD.md`); bridged to Rust via `X12LevelToProto`
- Surfaces: `proto/xcb/src/x12_surface.xml`; compositor `server/Xext/x12surface.c` (ADR-0013)
- Safe front-end: `safe/x12-proto` (ADR-0011/0012); `./tests/safe_proto/run.sh` (+ drift check)
- Vulkan demo: `./tests/surface/run_vk_present.sh` (lavapipe + memfd → Present)
- Workaround: Xvfb `-extension XFree86-Bigfont` until QueryFont/Bigfont is fixed

## After G1 (next sequence)

```text
A. [DONE] Present syncobj + MSC queue + Damage/stacking compose (ADR-0018); CopyArea on Xvfb
B. [DONE] X12-SCALE toolkit-opt-in stub (unity GetScale + OptIn; ADR-0018)
C. [DONE] Request matrix docs + XACE (cursor + all-device sandbox; ADR-0018)
D. [PARTIAL] Xephyr DRI3 screen_init + live Multiplane caps; real KMS atomic flips still open
E. HDR/ICC (v2)
F. Remoting — only if a new ADR reopens E*
G. [DONE] XML wire + fixed-request decode codegen + fuzz/ASAN (ADR-0016/0018)
H. [DONE] Policy: X11 = compat/test only; invest in native X12 path (ADR-0017)
I. [DONE] Reference WM: xwm (twm-class) + session smoke (ADR-0019)
```

## Explicitly after G1 (detail)

- DRM syncobj ioctl path when DDX provides import_syncobj
- Broader KMS desktop bring-up (Intel/AMD + proprietary NVIDIA atomic flips)
- Non-unity per-monitor scale policy once toolkits OptIn
- EWMH / richer WM features (only if a new ADR expands ADR-0019)
- HDR/ICC (v2)
- Remoting (only if new ADR reopens E*)
- Present/CreateSurface hand decode → generated (validators remain)

## Non-goals that must not sneak into G1

Wayland compositor identity, XWayland, XWin/XQuartz, full DE, remoting stack, HDR as a blocker.
