# X12 Roadmap — toward G1

**Owner:** Dimmus  
**Milestone G1:** Xvfb + xterm + capability deny keylog + one Vulkan client  
**Gate:** X11R8 legacy corpus green ([LEGACY_TESTS.md](LEGACY_TESTS.md))

## Sequence

```text
1. Import / vendor X11R8 baseline (meson tree, enough for Xvfb + xterm + legacy suite)
2. Stand up CI: meson build + ./tests/legacy/run.sh (fail on corpus regress)
3. Add hierarchical level assignment (auth token / CLI) + mediation hooks (XACE-style)
4. Security test: sandbox client cannot keylog full/user client
5. XML extension sketches: dmabuf surface + sync (XCB-style)
6. Memory-safe protocol front-end spike (language ADR) wrapping decode/validate
7. Built-in compositor path sufficient for Xvfb + one Vulkan dmabuf client
8. Tag G1 when demo script + legacy suite are green
```

## Explicitly after G1

- Toolkit-opt-in fractional / per-monitor scaling
- Broader KMS desktop bring-up (Intel/AMD + proprietary NVIDIA)
- Fine request-matrix documentation for sandbox/user/full
- HDR/ICC (v2)
- Remoting (only if new ADR reopens E*)

## Non-goals that must not sneak into G1

Wayland compositor identity, XWayland, XWin/XQuartz, full DE, remoting stack, HDR as a blocker.
