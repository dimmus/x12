# X12-SURFACE — dmabuf surface + explicit sync

**Extension-xname:** `X12-SURFACE`  
**IDL:** [`proto/xcb/src/x12_surface.xml`](../proto/xcb/src/x12_surface.xml)  
**ADR:** [ADR-0010](adr/0010-x12-surface-protocol.md)

## Purpose

Native presentation path for X12: clients hand the server **dmabuf-backed surfaces** and **timeline sync points**. Producers may be Vulkan, EGL/GL, or anything that exports dma-buf + DRM fourcc + modifier. The protocol does not name a GPU API.

Legacy DRI3/Present remain available for existing clients and the X11R8 corpus.

## Client flow (sketch)

```text
QueryVersion → QueryCapabilities → QueryModifiers(format)
  → CreateSurface(dmabuf FDs, format, modifier, geometry)
  → ImportSyncobj (optional, if Capability.Syncobj)
  → Attach(window, surface)
  → loop:
       render into buffer
       Present(window, surface, acquire/release points, serial, msc)
       wait CompleteNotify / IdleNotify
  → Detach / DestroySurface / FreeSyncobj
```

## Resources

| XID | Role |
|---|---|
| `SURFACE` | Imported multi-plane dmabuf + metadata |
| `SYNCOBJ` | DRM timeline syncobj (FD import) |
| `EVENT` | XGE subscription handle (`SelectInput`) |

## Relation to DRI3 / Present

| Concern | DRI3 + Present | X12-SURFACE |
|---|---|---|
| Buffer object | `PIXMAP` from dmabuf | `SURFACE` (no pixmap required) |
| Sync | Fence / syncobj via DRI3 | Syncobj in this extension |
| Present | `PresentPixmap` / `PixmapSynced` | `Present` on `SURFACE` |
| New Vulkan client | May use DRI3 | Preferred native path |

## Implementation status

| Piece | Status |
|---|---|
| XML IDL + meson/automake list | This step (ROADMAP 5) |
| Generated C/XCB bindings | `include/xcb/x12_surface.h`, `lib/xcb/proto/x12_surface.c` |
| Memory-safe decode front-end | Step 6 done — `safe/x12-proto` (ADR-0011) |
| Server ops + built-in compositor + Vulkan sample | Step 7 |

## Validation

```bash
xmllint --noout --schema proto/xcb/src/xcb.xsd proto/xcb/src/x12_surface.xml
# from a scratch dir (xcbgen writes .c/.h into cwd):
PYTHONPATH=proto/xcb python3 proto/xcb/xcbgen.py \
  -c xcb-proto -l x12_surface -s 3 proto/xcb/src/x12_surface.xml
```
