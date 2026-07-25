# ADR-0020: Unified Athena widget library (libXaw)

## Status

Accepted — 2026-07-25

Amended — 2026-07-25: sources live at `lib/src/xaw`; public headers at
`include/X11/Xaw` (no separate `Xaw3dxft` tree).

## Context

The tree vendored multiple Athena forks: classic Xaw, Xaw3d, Xaw3dxft,
XawPlus, neXtaw, XawM, and Mowitz. Apps only linked classic `libXaw`, while
duplicate libraries and header trees raised build cost and confusion.

The chosen lineage is Xaw3d with FreeType/Xft and UTF-8 (historically
packaged as Xaw3dxft) — the most capable Athena code that was in-tree.

## Decision

1. **One Athena library:** build `lib/src/xaw` as **`libXaw`** and export
   the Meson dependency **`libxaw`**.
2. **Headers:** install under `X11/Xaw/`. Clients `#include <X11/Xaw/...>`.
   Xft/UTF-8 APIs remain available via headers such as `Xaw3dXft.h` in that
   directory (names keep the upstream Xaw3dXft identifiers).
3. **Remove legacy forks** from the tree: classic `xaw`, `xaw3d`, `xawplus`,
   `nextaw`, `xaw3dm`, `mowitz`, and their
   `include/X11/{XawPlus,Xaw3d,neXtaw,Mowitz}` header trees (plus related
   `doc/devbook` material).
4. Consumers (`xterm`, `xclock`, `xfontsel`, demos) use `libxaw` and
   `#include <X11/Xaw/...>`.

## Consequences

- Single `-lXaw` / pkg-config `xaw7` for the project.
- Visual/API surface is Xaw3d+Xft (soname 8.5.x), not classic Xaw7 ABI.
- Alternate Athena skins are no longer available in-tree.

## References

- `lib/src/xaw/`, `include/X11/Xaw/`
- `doc/INFO_LIB.md`
