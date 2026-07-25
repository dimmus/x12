/*
 * X12-SURFACE: dmabuf surfaces + Xvfb compositor path (ADR-0013).
 * Decode/validate via Rust safe/x12-proto; Present CopyArea into attached window.
 */
#include <dix-config.h>

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dix/dix_priv.h"
#include "dixstruct_priv.h"
#include "extinit_priv.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "protocol-versions.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/x12surfaceproto.h>

#include "x12_proto.h"
#ifdef XACE
#include "x12level.h"
#endif

/* DRM fourcc / modifier without requiring libdrm for the extension unit. */
#define X12_FOURCC_XR24 0x34325258u /* DRM_FORMAT_XRGB8888 */
#define X12_FOURCC_AR24 0x34325241u /* DRM_FORMAT_ARGB8888 */
#define X12_MOD_LINEAR  0ull
#define X12_MOD_INVALID 0xffffffffffffffull

static int x12SurfaceErrorBase;
static int x12SurfaceReqCode;
static RESTYPE x12SurfaceType;
static RESTYPE x12SurfaceEventType;

typedef struct _X12Surface
{
    XID id;
    ClientPtr client;
    PixmapPtr pixmap;
    void *map;
    size_t map_size;
    uint16_t width;
    uint16_t height;
    uint8_t depth;
    uint8_t bpp;
    uint32_t format;
    uint64_t modifier;
    uint32_t stride;
    WindowPtr attached;
    struct _X12Surface *next;
} X12SurfaceRec, *X12SurfacePtr;

typedef struct _X12SurfaceEvent
{
    XID id;
    ClientPtr client;
    WindowPtr window;
    CARD32 mask;
    struct _X12SurfaceEvent *next;
} X12SurfaceEventRec, *X12SurfaceEventPtr;

static X12SurfaceEventPtr x12SurfaceEvents;
static X12SurfacePtr x12Surfaces;

static int
X12SurfaceFreeSurface(void *data, XID id)
{
    X12SurfacePtr s = data;
    X12SurfacePtr *prev;

    (void)id;
    if (!s)
        return Success;

    for (prev = &x12Surfaces; *prev; prev = &(*prev)->next) {
        if (*prev == s) {
            *prev = s->next;
            break;
        }
    }
    if (s->pixmap)
        (*s->pixmap->drawable.pScreen->DestroyPixmap)(s->pixmap);
    if (s->map && s->map != MAP_FAILED)
        munmap(s->map, s->map_size);
    free(s);
    return Success;
}

static int
X12SurfaceFreeEvent(void *data, XID id)
{
    X12SurfaceEventPtr ev = data;
    X12SurfaceEventPtr *prev;

    (void)id;
    for (prev = &x12SurfaceEvents; *prev; prev = &(*prev)->next) {
        if (*prev == ev) {
            *prev = ev->next;
            break;
        }
    }
    free(ev);
    return Success;
}

static int
X12SurfaceMapDecodeError(int32_t rc)
{
    switch (rc) {
    case X12_PROTO_OK:
        return Success;
    case X12_PROTO_TRUNCATED:
    case X12_PROTO_BAD_LENGTH:
        return BadLength;
    case X12_PROTO_DENIED:
    case X12_PROTO_BAD_LEVEL:
        return BadAccess;
    case X12_PROTO_MAJOR_MISMATCH:
    case X12_PROTO_UNKNOWN_OPCODE:
    case X12_PROTO_INVALID_FIELD:
    case X12_PROTO_FD_MISMATCH:
    case X12_PROTO_NULL:
    default:
        return BadValue;
    }
}

static void
X12SurfacePrepareFds(ClientPtr client)
{
    REQUEST(xReq);
    const uint8_t *raw = (const uint8_t *)client->requestBuffer;
    size_t bytes = (size_t)client->req_len * 4;

    /*
     * client->req_fds is only a consume counter (set via SetReqFds); it is not
     * filled by the transport. Peek the request so the Rust decoder sees the
     * expected FD count before we ReadFdFromClient.
     */
    switch (stuff->data) {
    case X_X12SurfaceCreateSurface:
        /* major,minor,len,surface,drawable,num_buffers */
        if (bytes >= 13)
            SetReqFds(client, raw[12]);
        break;
    case X_X12SurfaceImportSyncobj:
        SetReqFds(client, 1);
        break;
    default:
        break;
    }
}

static void
X12SurfaceDrainFds(ClientPtr client)
{
    while (client->req_fds > 0) {
        int fd = ReadFdFromClient(client);
        if (fd >= 0)
            close(fd);
    }
}

static int
X12SurfaceDecode(ClientPtr client, x12_proto_surface_decoded_t *out)
{
    REQUEST(xReq);
    size_t len = (size_t)client->req_len * 4;
    uint8_t level = X12_PROTO_LEVEL_FULL;
    int32_t rc;

#ifdef XACE
    level = X12LevelToProto(X12LevelOfClient(client));
#endif

    X12SurfacePrepareFds(client);
    rc = x12_proto_decode_surface((uint8_t)stuff->reqType,
                                  (const uint8_t *)client->requestBuffer,
                                  len,
                                  (uint32_t)client->req_fds,
                                  level,
                                  out);
    if (rc != X12_PROTO_OK) {
        X12SurfaceDrainFds(client);
        return X12SurfaceMapDecodeError(rc);
    }
    return Success;
}

static X12SurfacePtr
X12SurfaceLookup(ClientPtr client, XID id, int *rc)
{
    X12SurfacePtr s = NULL;

    *rc = dixLookupResourceByType((void **)&s, id, x12SurfaceType, client,
                                  DixReadAccess);
    if (*rc != Success)
        return NULL;
    return s;
}

static void
X12SurfaceSendComplete(WindowPtr window, XID surface, CARD32 serial, CARD8 mode)
{
    X12SurfaceEventPtr ev;
    CARD64 ust = GetTimeInMicros();

    for (ev = x12SurfaceEvents; ev; ev = ev->next) {
        xX12SurfaceCompleteNotify cn;

        if (ev->window != window)
            continue;
        if (!(ev->mask & X12SurfaceEventMaskCompleteNotify))
            continue;

        memset(&cn, 0, sizeof(cn));
        cn.type = GenericEvent;
        cn.extension = x12SurfaceReqCode;
        cn.sequenceNumber = ev->client->sequence;
        cn.length = (sz_xX12SurfaceCompleteNotify - 32) >> 2;
        cn.evtype = X12SurfaceNotifyComplete;
        cn.mode = mode;
        cn.eid = ev->id;
        cn.window = window->drawable.id;
        cn.serial = serial;
        cn.ust = ust;
        cn.msc = 0;
        cn.surface = surface;
        WriteEventsToClient(ev->client, 1, (xEvent *)&cn);
    }
}

static void
X12SurfaceSendIdle(WindowPtr window, XID surface, CARD32 serial)
{
    X12SurfaceEventPtr ev;

    for (ev = x12SurfaceEvents; ev; ev = ev->next) {
        xX12SurfaceIdleNotify in;

        if (ev->window != window)
            continue;
        if (!(ev->mask & X12SurfaceEventMaskIdleNotify))
            continue;

        memset(&in, 0, sizeof(in));
        in.type = GenericEvent;
        in.extension = x12SurfaceReqCode;
        in.sequenceNumber = ev->client->sequence;
        in.length = 0;
        in.evtype = X12SurfaceNotifyIdle;
        in.eid = ev->id;
        in.window = window->drawable.id;
        in.surface = surface;
        in.serial = serial;
        WriteEventsToClient(ev->client, 1, (xEvent *)&in);
    }
}

static int
ProcX12SurfaceQueryVersion(ClientPtr client, const x12_proto_surface_decoded_t *decoded)
{
    xX12SurfaceQueryVersionReply rep = {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .majorVersion = SERVER_X12SURFACE_MAJOR_VERSION,
        .minorVersion = SERVER_X12SURFACE_MINOR_VERSION,
    };

    (void)decoded;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.majorVersion);
        swapl(&rep.minorVersion);
    }
    WriteToClient(client, sizeof(rep), &rep);
    return Success;
}

static int
ProcX12SurfaceQueryCapabilities(ClientPtr client,
                                const x12_proto_surface_decoded_t *decoded)
{
    xX12SurfaceQueryCapabilitiesReply rep = {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .capabilities = X12SurfaceCapabilityMultiplane |
                        X12SurfaceCapabilityModifiers,
    };

    (void)decoded;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.capabilities);
    }
    WriteToClient(client, sizeof(rep), &rep);
    return Success;
}

static int
ProcX12SurfaceQueryModifiers(ClientPtr client,
                             const x12_proto_surface_decoded_t *decoded)
{
    uint32_t format = decoded->u.query_modifiers.format;
    CARD64 linear = X12_MOD_LINEAR;
    xX12SurfaceQueryModifiersReply rep;
    int n = 0;

    if (format == X12_FOURCC_XR24 || format == X12_FOURCC_AR24 || format == 0)
        n = 1;

    memset(&rep, 0, sizeof(rep));
    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = (CARD32)((n * 2 * sizeof(CARD64)) >> 2);
    rep.numWindowModifiers = (CARD32)n;
    rep.numScreenModifiers = (CARD32)n;

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.numWindowModifiers);
        swapl(&rep.numScreenModifiers);
    }
    WriteToClient(client, sizeof(rep), &rep);
    if (n) {
        WriteToClient(client, sizeof(linear), &linear);
        WriteToClient(client, sizeof(linear), &linear);
    }
    return Success;
}

static size_t
X12SurfaceMapSize(uint32_t stride, uint16_t height, uint32_t offset)
{
    return (size_t)offset + (size_t)stride * (size_t)height;
}

static int
ProcX12SurfaceCreateSurface(ClientPtr client,
                            const x12_proto_surface_decoded_t *decoded)
{
    const x12_proto_create_surface_t *cs = &decoded->u.create_surface;
    X12SurfacePtr s;
    DrawablePtr draw;
    ScreenPtr screen;
    int fd = -1;
    int rc;
    size_t map_size;
    void *map;
    PixmapPtr pixmap;
    int i;

    /* req_fds already set in X12SurfacePrepareFds */

    rc = dixLookupDrawable(&draw, cs->drawable, client, 0, DixGetAttrAccess);
    if (rc != Success)
        return rc;

    if (cs->num_buffers < 1 || cs->num_buffers > 4)
        return BadValue;
    if (cs->modifier != X12_MOD_LINEAR && cs->modifier != X12_MOD_INVALID)
        return BadMatch;
    if (cs->format != X12_FOURCC_XR24 && cs->format != X12_FOURCC_AR24)
        return BadMatch;
    if (cs->bpp != 32 || (cs->depth != 24 && cs->depth != 32))
        return BadMatch;
    if (cs->strides[0] < (uint32_t)cs->width * 4)
        return BadValue;

    LEGAL_NEW_RESOURCE(cs->surface, client);

    /* Consume plane FDs; G1 uses plane 0 only for the staging pixmap. */
    fd = ReadFdFromClient(client);
    if (fd < 0)
        return BadMatch;
    for (i = 1; i < cs->num_buffers; i++) {
        int extra = ReadFdFromClient(client);
        if (extra >= 0)
            close(extra);
    }

    map_size = X12SurfaceMapSize(cs->strides[0], cs->height, cs->offsets[0]);
    if (map_size == 0 || map_size > (size_t)512 * 1024 * 1024) {
        close(fd);
        return BadAlloc;
    }

    map = mmap(NULL, map_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (map == MAP_FAILED)
        return BadAccess;

    screen = draw->pScreen;
    pixmap = (*screen->CreatePixmap)(screen, 0, 0, cs->depth, 0);
    if (!pixmap) {
        munmap(map, map_size);
        return BadAlloc;
    }

    if (!(*screen->ModifyPixmapHeader)(pixmap, cs->width, cs->height, cs->depth,
                                       cs->bpp, (int)cs->strides[0],
                                       (char *)map + cs->offsets[0])) {
        (*screen->DestroyPixmap)(pixmap);
        munmap(map, map_size);
        return BadAlloc;
    }

    s = calloc(1, sizeof(*s));
    if (!s) {
        (*screen->DestroyPixmap)(pixmap);
        munmap(map, map_size);
        return BadAlloc;
    }

    s->id = cs->surface;
    s->client = client;
    s->pixmap = pixmap;
    s->map = map;
    s->map_size = map_size;
    s->width = cs->width;
    s->height = cs->height;
    s->depth = cs->depth;
    s->bpp = cs->bpp;
    s->format = cs->format;
    s->modifier = cs->modifier;
    s->stride = cs->strides[0];
    s->attached = NullWindow;
    s->next = x12Surfaces;
    x12Surfaces = s;

    if (!AddResource(cs->surface, x12SurfaceType, s))
        return BadAlloc;
    return Success;
}

static int
ProcX12SurfaceDestroySurface(ClientPtr client,
                             const x12_proto_surface_decoded_t *decoded)
{
    int rc;
    X12SurfacePtr s =
        X12SurfaceLookup(client, decoded->u.destroy_surface.surface, &rc);

    if (!s)
        return rc == BadValue ? x12SurfaceErrorBase + X12SurfaceBadSurface : rc;
    FreeResource(s->id, X11_RESTYPE_NONE);
    return Success;
}

static int
ProcX12SurfaceAttach(ClientPtr client, const x12_proto_surface_decoded_t *decoded)
{
    WindowPtr window;
    X12SurfacePtr s;
    int rc;

    rc = dixLookupWindow(&window, decoded->u.attach.window, client,
                         DixSetAttrAccess);
    if (rc != Success)
        return rc;
    s = X12SurfaceLookup(client, decoded->u.attach.surface, &rc);
    if (!s)
        return rc == BadValue ? x12SurfaceErrorBase + X12SurfaceBadSurface : rc;

    s->attached = window;
    return Success;
}

static int
ProcX12SurfaceDetach(ClientPtr client, const x12_proto_surface_decoded_t *decoded)
{
    WindowPtr window;
    X12SurfacePtr s;
    int rc;

    rc = dixLookupWindow(&window, decoded->u.detach.window, client,
                         DixSetAttrAccess);
    if (rc != Success)
        return rc;

    for (s = x12Surfaces; s; s = s->next) {
        if (s->client == client && s->attached == window)
            s->attached = NullWindow;
    }
    return Success;
}

static void
X12SurfaceCopyToWindow(X12SurfacePtr s, WindowPtr window, int16_t x_off,
                       int16_t y_off)
{
    ScreenPtr screen = window->drawable.pScreen;
    GCPtr gc;
    DrawablePtr dst = &window->drawable;

    gc = GetScratchGC(dst->depth, screen);
    if (!gc)
        return;
    ValidateGC(dst, gc);
    (void)(*gc->ops->CopyArea)(&s->pixmap->drawable, dst, gc, 0, 0, s->width,
                               s->height, x_off, y_off);
    FreeScratchGC(gc);
}

static int
ProcX12SurfacePresent(ClientPtr client, const x12_proto_surface_decoded_t *decoded)
{
    const x12_proto_present_t *p = &decoded->u.present;
    WindowPtr window;
    X12SurfacePtr s;
    int rc;

    rc = dixLookupWindow(&window, p->window, client, DixWriteAccess);
    if (rc != Success)
        return rc;
    s = X12SurfaceLookup(client, p->surface, &rc);
    if (!s)
        return rc == BadValue ? x12SurfaceErrorBase + X12SurfaceBadSurface : rc;

    if (!s->attached)
        s->attached = window;
    if (s->attached != window)
        return BadMatch;

    X12SurfaceCopyToWindow(s, window, p->x_off, p->y_off);
    X12SurfaceSendComplete(window, s->id, p->serial, X12SurfaceCompleteModeCopy);
    X12SurfaceSendIdle(window, s->id, p->serial);
    return Success;
}

static int
ProcX12SurfaceSelectInput(ClientPtr client,
                          const x12_proto_surface_decoded_t *decoded)
{
    const x12_proto_select_input_t *si = &decoded->u.select_input;
    WindowPtr window;
    X12SurfaceEventPtr ev;
    int rc;

    rc = dixLookupWindow(&window, si->window, client, DixReceiveAccess);
    if (rc != Success)
        return rc;

    LEGAL_NEW_RESOURCE(si->eid, client);
    ev = calloc(1, sizeof(*ev));
    if (!ev)
        return BadAlloc;
    ev->id = si->eid;
    ev->client = client;
    ev->window = window;
    ev->mask = si->event_mask;
    ev->next = x12SurfaceEvents;
    x12SurfaceEvents = ev;

    if (!AddResource(si->eid, x12SurfaceEventType, ev))
        return BadAlloc;
    return Success;
}

static int
ProcX12SurfaceDispatch(ClientPtr client)
{
    x12_proto_surface_decoded_t decoded;
    int rc;

    rc = X12SurfaceDecode(client, &decoded);
    if (rc != Success)
        return rc;

    switch (decoded.op) {
    case X_X12SurfaceQueryVersion:
        return ProcX12SurfaceQueryVersion(client, &decoded);
    case X_X12SurfaceQueryCapabilities:
        return ProcX12SurfaceQueryCapabilities(client, &decoded);
    case X_X12SurfaceQueryModifiers:
        return ProcX12SurfaceQueryModifiers(client, &decoded);
    case X_X12SurfaceCreateSurface:
        return ProcX12SurfaceCreateSurface(client, &decoded);
    case X_X12SurfaceDestroySurface:
        return ProcX12SurfaceDestroySurface(client, &decoded);
    case X_X12SurfaceImportSyncobj:
    case X_X12SurfaceFreeSyncobj:
        return BadImplementation;
    case X_X12SurfaceAttach:
        return ProcX12SurfaceAttach(client, &decoded);
    case X_X12SurfaceDetach:
        return ProcX12SurfaceDetach(client, &decoded);
    case X_X12SurfacePresent:
        return ProcX12SurfacePresent(client, &decoded);
    case X_X12SurfaceSelectInput:
        return ProcX12SurfaceSelectInput(client, &decoded);
    default:
        return BadRequest;
    }
}

static int _X_COLD
SProcX12SurfaceQueryVersion(ClientPtr client)
{
    REQUEST(xX12SurfaceQueryVersionReq);
    REQUEST_SIZE_MATCH(xX12SurfaceQueryVersionReq);
    swapl(&stuff->majorVersion);
    swapl(&stuff->minorVersion);
    return ProcX12SurfaceDispatch(client);
}

static int _X_COLD
SProcX12SurfaceDispatch(ClientPtr client)
{
    REQUEST(xReq);
    swaps(&stuff->length);
    switch (stuff->data) {
    case X_X12SurfaceQueryVersion:
        return SProcX12SurfaceQueryVersion(client);
    default:
        return ProcX12SurfaceDispatch(client);
    }
}

void
X12SurfaceExtensionInit(void)
{
    ExtensionEntry *extEntry;

    x12SurfaceType =
        CreateNewResourceType(X12SurfaceFreeSurface, "X12Surface");
    x12SurfaceEventType =
        CreateNewResourceType(X12SurfaceFreeEvent, "X12SurfaceEvent");
    if (!x12SurfaceType || !x12SurfaceEventType)
        FatalError("X12Surface: cannot allocate resource types\n");

    extEntry = AddExtension(X12SURFACE_NAME,
                            X12SURFACENumberEvents,
                            X12SURFACENumberErrors,
                            ProcX12SurfaceDispatch,
                            SProcX12SurfaceDispatch,
                            NULL,
                            StandardMinorOpcode);
    if (!extEntry) {
        ErrorF("X12Surface: AddExtension failed\n");
        return;
    }

    x12SurfaceErrorBase = extEntry->errorBase;
    x12SurfaceReqCode = extEntry->base;
    ErrorF("X12Surface: compositor path enabled (v%d.%d, mmap+CopyArea)\n",
           SERVER_X12SURFACE_MAJOR_VERSION,
           SERVER_X12SURFACE_MINOR_VERSION);
    ErrorF("X12Surface: front-end %s\n", x12_proto_version());
}
