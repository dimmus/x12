/*
 * X12-SURFACE: dmabuf surfaces + compositor path (ADR-0013 / ADR-0016 / ADR-0018).
 * Decode via Rust; CreateSurface prefers DRI3 pixmap_from_fds when available,
 * else Xvfb mmap+CopyArea. Present: software timeline syncobj + fake MSC queue,
 * damage/stacking compose (CopyArea until real KMS flips land).
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
#include "os.h"
#include "pixmapstr.h"
#include "protocol-versions.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/x12surfaceproto.h>

#include "x12_proto.h"
#ifdef XACE
#include "x12level.h"
#endif
#ifdef DRI3
#include "../dri3/dri3_priv.h"
#endif
#ifdef DAMAGE
#include "damage.h"
#endif

/* DRM fourcc / modifier without requiring libdrm for the extension unit. */
#define X12_FOURCC_XR24 0x34325258u /* DRM_FORMAT_XRGB8888 */
#define X12_FOURCC_AR24 0x34325241u /* DRM_FORMAT_ARGB8888 */
#define X12_MOD_LINEAR  0ull
#define X12_MOD_INVALID 0xffffffffffffffull

/* ~60Hz software compositor clock when no KMS vblank is available. */
#define X12_SURFACE_FAKE_INTERVAL_US 16667ull

#define X12SurfacePresentOptionAsync    1
#define X12SurfacePresentOptionCopy     2
#define X12SurfacePresentOptionTearFree 4

static int x12SurfaceErrorBase;
static int x12SurfaceReqCode;
static RESTYPE x12SurfaceType;
static RESTYPE x12SurfaceEventType;
static RESTYPE x12SyncobjType;

typedef struct _X12Surface
{
    XID id;
    ClientPtr client;
    PixmapPtr pixmap;
    void *map;
    size_t map_size;
    Bool dri3_backed; /* pixmap owns storage; do not munmap */
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

typedef struct _X12Syncobj
{
    XID id;
    ClientPtr client;
    uint64_t point; /* software timeline point */
    struct _X12Syncobj *next;
} X12SyncobjRec, *X12SyncobjPtr;

typedef struct _X12SurfaceEvent
{
    XID id;
    ClientPtr client;
    WindowPtr window;
    CARD32 mask;
    struct _X12SurfaceEvent *next;
} X12SurfaceEventRec, *X12SurfaceEventPtr;

typedef struct _X12PendingPresent
{
    ClientPtr client;
    WindowPtr window;
    X12SurfacePtr surface;
    CARD32 serial;
    int16_t x_off;
    int16_t y_off;
    XID release_syncobj;
    uint64_t release_point;
    uint64_t target_msc;
    OsTimerPtr timer;
    struct _X12PendingPresent *next;
} X12PendingPresentRec, *X12PendingPresentPtr;

static X12SurfaceEventPtr x12SurfaceEvents;
static X12SurfacePtr x12Surfaces;
static X12SyncobjPtr x12Syncobjs;
static X12PendingPresentPtr x12PendingPresents;

static void X12SurfaceExecutePresent(X12PendingPresentPtr pp, uint64_t ust,
                                     uint64_t msc);
static void X12SurfaceFlushPendingForMsc(ScreenPtr screen, uint64_t msc);

static int
X12SurfaceFreeSurface(void *data, XID id)
{
    X12SurfacePtr s = data;
    X12SurfacePtr *prev;
    X12PendingPresentPtr *pp, doomed;

    (void)id;
    if (!s)
        return Success;

    /* Drop queued presents that still reference this surface. */
    for (pp = &x12PendingPresents; *pp;) {
        if ((*pp)->surface == s) {
            doomed = *pp;
            *pp = doomed->next;
            if (doomed->timer)
                TimerFree(doomed->timer);
            free(doomed);
            continue;
        }
        pp = &(*pp)->next;
    }

    for (prev = &x12Surfaces; *prev; prev = &(*prev)->next) {
        if (*prev == s) {
            *prev = s->next;
            break;
        }
    }
    if (s->pixmap)
        (*s->pixmap->drawable.pScreen->DestroyPixmap)(s->pixmap);
    if (!s->dri3_backed && s->map && s->map != MAP_FAILED)
        munmap(s->map, s->map_size);
    free(s);
    return Success;
}

static int
X12SurfaceFreeSyncobj(void *data, XID id)
{
    X12SyncobjPtr so = data;
    X12SyncobjPtr *prev;

    (void)id;
    if (!so)
        return Success;
    for (prev = &x12Syncobjs; *prev; prev = &(*prev)->next) {
        if (*prev == so) {
            *prev = so->next;
            break;
        }
    }
    free(so);
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

static X12SyncobjPtr
X12SyncobjLookup(ClientPtr client, XID id, int *rc)
{
    X12SyncobjPtr so = NULL;

    *rc = dixLookupResourceByType((void **)&so, id, x12SyncobjType, client,
                                  DixReadAccess);
    if (*rc != Success)
        return NULL;
    return so;
}

static void
X12SurfaceGetUstMsc(ScreenPtr screen, uint64_t *ust, uint64_t *msc)
{
    (void)screen;
    *ust = GetTimeInMicros();
    *msc = (*ust + X12_SURFACE_FAKE_INTERVAL_US / 2) / X12_SURFACE_FAKE_INTERVAL_US;
}

static uint64_t
X12SurfaceTargetMsc(uint64_t target_msc_arg,
                    uint64_t crtc_msc,
                    uint64_t divisor,
                    uint64_t remainder,
                    uint32_t options)
{
    const Bool synced = !(options & X12SurfacePresentOptionAsync);
    uint64_t target_msc;

    if (target_msc_arg > crtc_msc)
        return target_msc_arg;

    if (divisor == 0) {
        target_msc = crtc_msc;
        if (synced)
            target_msc++;
        return target_msc;
    }

    if (remainder >= divisor)
        remainder = 0;

    target_msc = crtc_msc - (crtc_msc % divisor) + remainder;
    if (target_msc > crtc_msc)
        return target_msc;
    if (synced || crtc_msc > target_msc)
        target_msc += divisor;
    return target_msc;
}

static Bool
X12SurfaceScreenHasDri3Import(ScreenPtr screen)
{
#ifdef DRI3
    dri3_screen_priv_ptr ds;

    if (!screen)
        return FALSE;
    ds = dri3_screen_priv(screen);
    if (!ds || !ds->info)
        return FALSE;
    if (ds->info->version >= 2 && ds->info->pixmap_from_fds)
        return TRUE;
    if (ds->info->pixmap_from_fd)
        return TRUE;
#else
    (void)screen;
#endif
    return FALSE;
}

static CARD32
X12SurfaceCapsForScreen(ScreenPtr screen)
{
    CARD32 caps = X12SurfaceCapabilitySyncobj | X12SurfaceCapabilityModifiers;

    if (X12SurfaceScreenHasDri3Import(screen))
        caps |= X12SurfaceCapabilityMultiplane;
    return caps;
}

static int
X12SurfaceWindowStackDepth(WindowPtr w)
{
    int depth = 0;
    WindowPtr p;

    for (p = w; p; p = p->parent)
        depth++;
    return depth;
}

/* Higher stacking order first among siblings; deeper windows before ancestors. */
static int
X12SurfaceComparePending(const X12PendingPresentPtr a,
                         const X12PendingPresentPtr b)
{
    WindowPtr wa = a->window;
    WindowPtr wb = b->window;
    WindowPtr pa, pb, sa, sb;

    if (wa == wb)
        return 0;

    /* Find common ancestor's children for sibling order. */
    pa = wa;
    pb = wb;
    while (X12SurfaceWindowStackDepth(pa) > X12SurfaceWindowStackDepth(pb))
        pa = pa->parent;
    while (X12SurfaceWindowStackDepth(pb) > X12SurfaceWindowStackDepth(pa))
        pb = pb->parent;
    while (pa && pb && pa->parent != pb->parent) {
        pa = pa->parent;
        pb = pb->parent;
    }
    if (pa && pb && pa->parent == pb->parent) {
        /* Walk sibling list: later siblings are above. Bottom paints first. */
        for (sa = pa->parent ? pa->parent->firstChild : pa; sa; sa = sa->nextSib) {
            if (sa == pa)
                return -1; /* a under b → a first */
            if (sa == pb)
                return 1;
        }
    }
    return (int)((uintptr_t)wa > (uintptr_t)wb) - (int)((uintptr_t)wa < (uintptr_t)wb);
}

static void
X12SurfaceSendComplete(WindowPtr window, XID surface, CARD32 serial, CARD8 mode,
                       uint64_t ust, uint64_t msc)
{
    X12SurfaceEventPtr ev;

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
        cn.msc = msc;
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

static void
X12SurfaceCopyToWindow(X12SurfacePtr s, WindowPtr window, int16_t x_off,
                       int16_t y_off)
{
    ScreenPtr screen = window->drawable.pScreen;
    GCPtr gc;
    DrawablePtr dst = &window->drawable;
#ifdef DAMAGE
    BoxRec box;
    RegionRec region;
#endif

    gc = GetScratchGC(dst->depth, screen);
    if (!gc)
        return;
    ValidateGC(dst, gc);
    (void)(*gc->ops->CopyArea)(&s->pixmap->drawable, dst, gc, 0, 0, s->width,
                               s->height, x_off, y_off);
    FreeScratchGC(gc);

#ifdef DAMAGE
    /* Mark composed region damaged so listeners / compositors see the update. */
    box.x1 = x_off;
    box.y1 = y_off;
    box.x2 = x_off + (INT16)s->width;
    box.y2 = y_off + (INT16)s->height;
    RegionInit(&region, &box, 1);
    DamageDamageRegion(dst, &region);
    RegionUninit(&region);
#endif
}

static void
X12SurfaceSignalSyncobj(ClientPtr client, XID id, uint64_t point)
{
    X12SyncobjPtr so;
    int rc;

    if (!id || !point)
        return;
    so = X12SyncobjLookup(client, id, &rc);
    if (!so)
        return;
    if (point > so->point)
        so->point = point;
}

static Bool
X12SurfaceAcquireReady(ClientPtr client, XID id, uint64_t point, int *err)
{
    X12SyncobjPtr so;
    int rc;

    *err = Success;
    if (!id)
        return TRUE;
    so = X12SyncobjLookup(client, id, &rc);
    if (!so) {
        *err = (rc == BadValue) ? x12SurfaceErrorBase + X12SurfaceBadSyncobj : rc;
        return FALSE;
    }
    if (point == 0)
        return TRUE;
    if (so->point >= point)
        return TRUE;
    *err = BadMatch;
    return FALSE;
}

static void
X12SurfaceExecutePresent(X12PendingPresentPtr pp, uint64_t ust, uint64_t msc)
{
    CARD8 mode = X12SurfaceCompleteModeCopy;

    if (!pp || !pp->surface || !pp->window)
        return;

    X12SurfaceCopyToWindow(pp->surface, pp->window, pp->x_off, pp->y_off);
    X12SurfaceSignalSyncobj(pp->client, pp->release_syncobj, pp->release_point);
    X12SurfaceSendComplete(pp->window, pp->surface->id, pp->serial, mode, ust,
                           msc);
    X12SurfaceSendIdle(pp->window, pp->surface->id, pp->serial);
}

static void
X12SurfaceFlushPendingForMsc(ScreenPtr screen, uint64_t msc)
{
    X12PendingPresentPtr ready[32];
    int n = 0, i, j;
    X12PendingPresentPtr *pp, cur;
    uint64_t ust, now_msc;

    X12SurfaceGetUstMsc(screen, &ust, &now_msc);
    if (msc < now_msc)
        msc = now_msc;

    for (pp = &x12PendingPresents; *pp;) {
        cur = *pp;
        if (cur->window->drawable.pScreen != screen || cur->target_msc > msc) {
            pp = &(*pp)->next;
            continue;
        }
        *pp = cur->next;
        if (cur->timer) {
            TimerFree(cur->timer);
            cur->timer = NULL;
        }
        if (n < 32)
            ready[n++] = cur;
        else {
            X12SurfaceExecutePresent(cur, ust, msc);
            free(cur);
        }
    }

    /* Stacking order: bottom to top so higher windows win the last CopyArea. */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (X12SurfaceComparePending(ready[i], ready[j]) < 0) {
                X12PendingPresentPtr tmp = ready[i];
                ready[i] = ready[j];
                ready[j] = tmp;
            }
        }
    }
    for (i = 0; i < n; i++) {
        X12SurfaceExecutePresent(ready[i], ust, msc);
        free(ready[i]);
    }
}

static CARD32
X12SurfacePendingTimer(OsTimerPtr timer, CARD32 time, void *arg)
{
    X12PendingPresentPtr pp = arg;
    ScreenPtr screen;

    (void)timer;
    (void)time;
    if (!pp || !pp->window)
        return 0;
    screen = pp->window->drawable.pScreen;
    pp->timer = NULL;
    X12SurfaceFlushPendingForMsc(screen, pp->target_msc);
    return 0;
}

static int
X12SurfaceQueuePresent(ClientPtr client,
                       WindowPtr window,
                       X12SurfacePtr surface,
                       const x12_proto_present_t *p,
                       uint64_t target_msc)
{
    X12PendingPresentPtr pp;
    uint64_t ust, now_msc;
    INT32 delay_ms;

    X12SurfaceGetUstMsc(window->drawable.pScreen, &ust, &now_msc);

    pp = calloc(1, sizeof(*pp));
    if (!pp)
        return BadAlloc;
    pp->client = client;
    pp->window = window;
    pp->surface = surface;
    pp->serial = p->serial;
    pp->x_off = p->x_off;
    pp->y_off = p->y_off;
    pp->release_syncobj = p->release_syncobj;
    pp->release_point = p->release_point;
    pp->target_msc = target_msc;
    pp->next = x12PendingPresents;
    x12PendingPresents = pp;

    if (target_msc <= now_msc) {
        X12SurfaceFlushPendingForMsc(window->drawable.pScreen, now_msc);
        return Success;
    }

    delay_ms = (INT32)((target_msc - now_msc) * X12_SURFACE_FAKE_INTERVAL_US / 1000);
    if (delay_ms < 1)
        delay_ms = 1;
    pp->timer = TimerSet(NULL, 0, (CARD32)delay_ms, X12SurfacePendingTimer, pp);
    if (!pp->timer) {
        /* Fall back to immediate compose. */
        X12SurfaceFlushPendingForMsc(window->drawable.pScreen, target_msc);
    }
    return Success;
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
    DrawablePtr draw;
    int rc;
    xX12SurfaceQueryCapabilitiesReply rep;

    rc = dixLookupDrawable(&draw, decoded->u.query_capabilities.drawable, client,
                           0, DixGetAttrAccess);
    if (rc != Success)
        return rc;

    memset(&rep, 0, sizeof(rep));
    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    rep.capabilities = X12SurfaceCapsForScreen(draw->pScreen);

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
    int fds[4];
    int nfd = 0;
    int rc;
    size_t map_size;
    void *map = NULL;
    PixmapPtr pixmap = NullPixmap;
    Bool dri3_backed = FALSE;
    int i;

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

    for (i = 0; i < cs->num_buffers; i++) {
        fds[i] = ReadFdFromClient(client);
        if (fds[i] < 0) {
            while (--i >= 0)
                close(fds[i]);
            return BadMatch;
        }
        nfd++;
    }

    screen = draw->pScreen;

#ifdef DRI3
    if (X12SurfaceScreenHasDri3Import(screen)) {
        CARD32 strides[4], offsets[4];
        for (i = 0; i < nfd; i++) {
            strides[i] = cs->strides[i];
            offsets[i] = cs->offsets[i];
        }
        rc = dri3_pixmap_from_fds(&pixmap, screen, (CARD8)nfd, fds,
                                  cs->width, cs->height, strides, offsets,
                                  cs->depth, cs->bpp, cs->modifier);
        if (rc == Success && pixmap) {
            dri3_backed = TRUE;
            for (i = 0; i < nfd; i++)
                close(fds[i]);
            nfd = 0;
        }
        else {
            pixmap = NullPixmap;
        }
    }
#endif

    if (!dri3_backed) {
        /* Xvfb / no DRI3: mmap plane 0 (LINEAR host-visible). */
        map_size = X12SurfaceMapSize(cs->strides[0], cs->height, cs->offsets[0]);
        if (map_size == 0 || map_size > (size_t)512 * 1024 * 1024) {
            for (i = 0; i < nfd; i++)
                close(fds[i]);
            return BadAlloc;
        }
        map = mmap(NULL, map_size, PROT_READ, MAP_SHARED, fds[0], 0);
        for (i = 0; i < nfd; i++)
            close(fds[i]);
        nfd = 0;
        if (map == MAP_FAILED)
            return BadAccess;

        pixmap = (*screen->CreatePixmap)(screen, 0, 0, cs->depth, 0);
        if (!pixmap) {
            munmap(map, map_size);
            return BadAlloc;
        }
        if (!(*screen->ModifyPixmapHeader)(pixmap, cs->width, cs->height,
                                           cs->depth, cs->bpp,
                                           (int)cs->strides[0],
                                           (char *)map + cs->offsets[0])) {
            (*screen->DestroyPixmap)(pixmap);
            munmap(map, map_size);
            return BadAlloc;
        }
    }

    s = calloc(1, sizeof(*s));
    if (!s) {
        (*screen->DestroyPixmap)(pixmap);
        if (!dri3_backed && map && map != MAP_FAILED)
            munmap(map, map_size);
        return BadAlloc;
    }

    s->id = cs->surface;
    s->client = client;
    s->pixmap = pixmap;
    s->map = dri3_backed ? NULL : map;
    s->map_size = dri3_backed ? 0 : X12SurfaceMapSize(cs->strides[0], cs->height,
                                                       cs->offsets[0]);
    s->dri3_backed = dri3_backed;
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

    DebugF("X12Surface: CreateSurface id=0x%x %s %ux%u\n",
           (unsigned)cs->surface, dri3_backed ? "dri3" : "mmap",
           cs->width, cs->height);

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
ProcX12SurfaceImportSyncobj(ClientPtr client,
                            const x12_proto_surface_decoded_t *decoded)
{
    const x12_proto_import_syncobj_t *is = &decoded->u.import_syncobj;
    DrawablePtr draw;
    X12SyncobjPtr so;
    int fd;
    int rc;

    rc = dixLookupDrawable(&draw, is->drawable, client, 0, DixGetAttrAccess);
    if (rc != Success) {
        X12SurfaceDrainFds(client);
        return rc;
    }
    (void)draw;

    LEGAL_NEW_RESOURCE(is->syncobj, client);

    fd = ReadFdFromClient(client);
    if (fd < 0)
        return BadMatch;
    /*
     * Software timeline stub (Xvfb / no DRM syncobj DDX). Accept any FD as
     * the Import handshake, then close it. Timeline advances via Present
     * release_point (and future DRM ioctl path when DDX provides it).
     */
    close(fd);

    so = calloc(1, sizeof(*so));
    if (!so)
        return BadAlloc;
    so->id = is->syncobj;
    so->client = client;
    so->point = 0;
    so->next = x12Syncobjs;
    x12Syncobjs = so;

    if (!AddResource(is->syncobj, x12SyncobjType, so))
        return BadAlloc;
    return Success;
}

static int
ProcX12SurfaceFreeSyncobj(ClientPtr client,
                          const x12_proto_surface_decoded_t *decoded)
{
    int rc;
    X12SyncobjPtr so =
        X12SyncobjLookup(client, decoded->u.free_syncobj.syncobj, &rc);

    if (!so)
        return rc == BadValue ? x12SurfaceErrorBase + X12SurfaceBadSyncobj : rc;
    FreeResource(so->id, X11_RESTYPE_NONE);
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

static int
ProcX12SurfacePresent(ClientPtr client, const x12_proto_surface_decoded_t *decoded)
{
    const x12_proto_present_t *p = &decoded->u.present;
    WindowPtr window;
    X12SurfacePtr s;
    int rc;
    uint64_t ust, now_msc, target;

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

    if (p->options &
        ~(X12SurfacePresentOptionAsync | X12SurfacePresentOptionCopy |
          X12SurfacePresentOptionTearFree))
        return BadValue;

    if (!X12SurfaceAcquireReady(client, p->acquire_syncobj, p->acquire_point,
                                &rc))
        return rc;
    if (p->release_syncobj) {
        X12SyncobjPtr so = X12SyncobjLookup(client, p->release_syncobj, &rc);
        if (!so)
            return (rc == BadValue)
                       ? x12SurfaceErrorBase + X12SurfaceBadSyncobj
                       : rc;
    }

    X12SurfaceGetUstMsc(window->drawable.pScreen, &ust, &now_msc);
    target = X12SurfaceTargetMsc(p->target_msc, now_msc, p->divisor, p->remainder,
                                 p->options);

    return X12SurfaceQueuePresent(client, window, s, p, target);
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
        return ProcX12SurfaceImportSyncobj(client, &decoded);
    case X_X12SurfaceFreeSyncobj:
        return ProcX12SurfaceFreeSyncobj(client, &decoded);
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
    x12SyncobjType =
        CreateNewResourceType(X12SurfaceFreeSyncobj, "X12Syncobj");
    if (!x12SurfaceType || !x12SurfaceEventType || !x12SyncobjType)
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
    ErrorF("X12Surface: compositor path enabled (v%d.%d, syncobj+MSC+CopyArea)\n",
           SERVER_X12SURFACE_MAJOR_VERSION,
           SERVER_X12SURFACE_MINOR_VERSION);
    ErrorF("X12Surface: front-end %s\n", x12_proto_version());
}
