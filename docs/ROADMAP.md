# X12 Roadmap — toward G1

**Owner:** Dimmus  
**Milestone G1:** Xvfb + xterm + capability deny keylog + one Vulkan client  
**Gate:** X11R8 legacy corpus green ([LEGACY_TESTS.md](LEGACY_TESTS.md))

## Sequence

```text
1. [DONE] Import / vendor X11R8 baseline (meson tree, Xvfb + xterm + legacy suite)
2. [DONE] Stand up CI: meson build + ./tests/legacy/run.sh (fail on corpus regress)
3. Add hierarchical level assignment (auth token / CLI) + mediation hooks (XACE-style)
4. Security test: sandbox client cannot keylog full/user client
5. XML extension sketches: dmabuf surface + sync (XCB-style)
6. Memory-safe protocol front-end spike (language ADR) wrapping decode/validate
7. Built-in compositor path sufficient for Xvfb + one Vulkan dmabuf client
8. Tag G1 when demo script + legacy suite are green
```

### Step 1 notes (2026-07-25)

- Provenance: ADR-0008 / `docs/vendor/X11R8-README.md`
- Legacy harness: `./tests/legacy/run.sh` (xauth + meson unit/server + x11perf + xcmstest + xterm)
- CI: `.github/workflows/legacy.yml`
- Workaround: Xvfb `-extension XFree86-Bigfont` until QueryFont/Bigfont is fixed


## Explicitly after G1

- Toolkit-opt-in fractional / per-monitor scaling
- Broader KMS desktop bring-up (Intel/AMD + proprietary NVIDIA)
- Fine request-matrix documentation for sandbox/user/full
- HDR/ICC (v2)
- Remoting (only if new ADR reopens E*)

## Non-goals that must not sneak into G1

Wayland compositor identity, XWayland, XWin/XQuartz, full DE, remoting stack, HDR as a blocker.
