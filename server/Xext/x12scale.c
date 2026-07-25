/*
 * X12-SCALE: toolkit-opt-in fractional / per-monitor scaling (ADR-0005 D4).
 * Stub: reports unity scale; records OptIn per window. No silent blur-scale.
 */
#include <dix-config.h>

#include <string.h>

#include "dix/dix_priv.h"
#include "dixstruct_priv.h"
#include "extinit_priv.h"
#include "protocol-versions.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/x12scaleproto.h>

#ifndef SERVER_X12SCALE_MAJOR_VERSION
#define SERVER_X12SCALE_MAJOR_VERSION X12SCALE_MAJOR
#endif
#ifndef SERVER_X12SCALE_MINOR_VERSION
#define SERVER_X12SCALE_MINOR_VERSION X12SCALE_MINOR
#endif

typedef struct _X12ScaleOptIn
{
    WindowPtr window;
    Bool enabled;
    struct _X12ScaleOptIn *next;
} X12ScaleOptInRec, *X12ScaleOptInPtr;

static X12ScaleOptInPtr x12ScaleOptIns;

static X12ScaleOptInPtr
X12ScaleFindOptIn(WindowPtr window)
{
    X12ScaleOptInPtr st;

    for (st = x12ScaleOptIns; st; st = st->next) {
        if (st->window == window)
            return st;
    }
    return NULL;
}

static int
ProcX12ScaleQueryVersion(ClientPtr client)
{
    REQUEST(xX12ScaleQueryVersionReq);
    xX12ScaleQueryVersionReply rep = {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .majorVersion = SERVER_X12SCALE_MAJOR_VERSION,
        .minorVersion = SERVER_X12SCALE_MINOR_VERSION,
    };

    REQUEST_SIZE_MATCH(xX12ScaleQueryVersionReq);
    (void)stuff;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.majorVersion);
        swapl(&rep.minorVersion);
    }
    WriteToClient(client, sizeof(rep), &rep);
    return Success;
}

static int
ProcX12ScaleGetScale(ClientPtr client)
{
    REQUEST(xX12ScaleGetScaleReq);
    WindowPtr window;
    ScreenPtr screen;
    int rc;
    xX12ScaleGetScaleReply rep;

    REQUEST_SIZE_MATCH(xX12ScaleGetScaleReq);
    rc = dixLookupWindow(&window, stuff->window, client, DixGetAttrAccess);
    if (rc != Success)
        return rc;

    screen = window->drawable.pScreen;
    memset(&rep, 0, sizeof(rep));
    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    /* Unity until a monitor policy + toolkit OptIn drives non-1.0. */
    rep.scaleX = X12ScaleUnity;
    rep.scaleY = X12ScaleUnity;
    rep.widthMm = screen->mmWidth;
    rep.heightMm = screen->mmHeight;

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.scaleX);
        swapl(&rep.scaleY);
        swaps(&rep.widthMm);
        swaps(&rep.heightMm);
    }
    WriteToClient(client, sizeof(rep), &rep);
    return Success;
}

static int
ProcX12ScaleOptIn(ClientPtr client)
{
    REQUEST(xX12ScaleOptInReq);
    WindowPtr window;
    X12ScaleOptInPtr st;
    int rc;

    REQUEST_SIZE_MATCH(xX12ScaleOptInReq);
    rc = dixLookupWindow(&window, stuff->window, client, DixSetAttrAccess);
    if (rc != Success)
        return rc;

    st = X12ScaleFindOptIn(window);
    if (!st) {
        st = calloc(1, sizeof(*st));
        if (!st)
            return BadAlloc;
        st->window = window;
        st->next = x12ScaleOptIns;
        x12ScaleOptIns = st;
    }
    st->enabled = stuff->enable ? TRUE : FALSE;
    return Success;
}

static int
ProcX12ScaleDispatch(ClientPtr client)
{
    REQUEST(xReq);

    switch (stuff->data) {
    case X_X12ScaleQueryVersion:
        return ProcX12ScaleQueryVersion(client);
    case X_X12ScaleGetScale:
        return ProcX12ScaleGetScale(client);
    case X_X12ScaleOptIn:
        return ProcX12ScaleOptIn(client);
    default:
        return BadRequest;
    }
}

static int _X_COLD
SProcX12ScaleDispatch(ClientPtr client)
{
    REQUEST(xReq);
    swaps(&stuff->length);
    switch (stuff->data) {
    case X_X12ScaleQueryVersion: {
        REQUEST(xX12ScaleQueryVersionReq);
        swapl(&stuff->majorVersion);
        swapl(&stuff->minorVersion);
        return ProcX12ScaleQueryVersion(client);
    }
    case X_X12ScaleGetScale: {
        REQUEST(xX12ScaleGetScaleReq);
        swapl(&stuff->window);
        return ProcX12ScaleGetScale(client);
    }
    case X_X12ScaleOptIn: {
        REQUEST(xX12ScaleOptInReq);
        swapl(&stuff->window);
        return ProcX12ScaleOptIn(client);
    }
    default:
        return BadRequest;
    }
}

void
X12ScaleExtensionInit(void)
{
    ExtensionEntry *extEntry;

    extEntry = AddExtension(X12SCALE_NAME,
                            X12SCALENumberEvents,
                            X12SCALENumberErrors,
                            ProcX12ScaleDispatch,
                            SProcX12ScaleDispatch,
                            NULL,
                            StandardMinorOpcode);
    if (!extEntry) {
        ErrorF("X12Scale: AddExtension failed\n");
        return;
    }
    ErrorF("X12Scale: toolkit opt-in scaling stub (v%d.%d, unity)\n",
           SERVER_X12SCALE_MAJOR_VERSION, SERVER_X12SCALE_MINOR_VERSION);
}
