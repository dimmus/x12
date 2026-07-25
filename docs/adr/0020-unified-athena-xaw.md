# ADR-0020: Unified Athena widget library (libXaw = Xaw3dxft)

## Status

Accepted — 2026-07-25

## Context

The tree vendored multiple Athena forks: classic Xaw, Xaw3d, Xaw3dxft,
XawPlus, neXtaw, XawM, and Mowitz. Apps only linked classic `libXaw`, while
duplicate libraries and header trees raised build cost and confusion.

Xaw3dxft is source-compatible with Xaw3d and adds FreeType/Xft and UTF-8 —
the most capable Athena lineage that was in-tree.

## Decision

1. **One Athena library:** build `lib/src/xaw3dxft` as **`libXaw`** and export
   the Meson dependency **`libxaw`**.
2. **Headers:** install implementation headers under `X11/Xaw3dxft/`. Provide
   `X11/Xaw/*.h` as thin shims that `#include` the Xaw3dxft counterparts so
   existing clients keep compiling.
3. **Remove legacy forks** from the tree: classic `xaw`, `xaw3d`, `xawplus`,
   `nextaw`, `xaw3dm`, `mowitz`, and their `include/X11/{XawPlus,Xaw3d,neXtaw,Mowitz}`
   header trees (plus related `doc/devbook` material).
4. Consumers (`xterm`, `xclock`, `xfontsel`, demos) use `libxaw` and
   `#include <X11/Xaw/...>`.

## Consequences

- Single `-lXaw` / pkg-config `xaw7` for the project.
- Visual/API surface is Xaw3d+Xft (soname 8.5.x), not classic Xaw7 ABI.
- Alternate Athena skins are no longer available in-tree.

## References

- `lib/src/xaw3dxft/`, `include/X11/Xaw/`, `include/X11/Xaw3dxft/`
- `doc/INFO_LIB.md`
