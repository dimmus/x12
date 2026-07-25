/* SPDX-License-Identifier: MIT
 *
 * X12 hierarchical client levels via XACE (ADR-0003, ADR-0016).
 *
 * Levels: sandbox (0) < user (1) < full (2)
 * Default: full (-client-level)
 *
 * Request matrix (post-G1):
 *  sandbox — no keyboard read/grab/write; no foreign receive; no foreign
 *            GetImage/drawable read; no foreign selection/property sniff;
 *            no XTEST/RECORD dispatch
 *  user    — no foreign GetImage; no foreign selection sniff; no XTEST;
 *            keyboard QueryKeymap allowed
 *  full    — unrestricted (legacy ambient trust)
 */

#include <dix-config.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dix/dix_priv.h"
#include "include/extinit_priv.h"
#include "os/osdep.h"

#include "inputstr.h"
#include "windowstr.h"
#include "privates.h"
#include "dixstruct.h"
#include "resource.h"
#include "selection.h"
#include "xacestr.h"
#include "xace.h"
#include "x12level.h"
#include "extinit.h"
#include "extension.h"

int x12DefaultClientLevel = X12_LEVEL_FULL;

static DevPrivateKeyRec x12LevelKeyRec;
#define x12LevelKey (&x12LevelKeyRec)

typedef struct {
    int level;
} X12LevelStateRec;

#define X12_MAX_FORCE 32
static int x12ForceSandbox[X12_MAX_FORCE];
static int x12ForceSandboxCount = 0;

int
X12LevelParseName(const char *name)
{
    if (!name)
        return -1;
    if (!strcmp(name, "sandbox"))
        return X12_LEVEL_SANDBOX;
    if (!strcmp(name, "user"))
        return X12_LEVEL_USER;
    if (!strcmp(name, "full"))
        return X12_LEVEL_FULL;
    return -1;
}

void
X12LevelParseSandboxClients(const char *list)
{
    char *copy, *tok, *save = NULL;

    x12ForceSandboxCount = 0;
    if (!list || !list[0])
        return;
    copy = strdup(list);
    if (!copy)
        return;
    for (tok = strtok_r(copy, ",", &save); tok;
         tok = strtok_r(NULL, ",", &save)) {
        char *end = NULL;
        long v;

        while (*tok && isspace((unsigned char)*tok))
            tok++;
        if (!*tok)
            continue;
        v = strtol(tok, &end, 10);
        if (end == tok || v < 0 || v > 512)
            continue;
        if (x12ForceSandboxCount < X12_MAX_FORCE)
            x12ForceSandbox[x12ForceSandboxCount++] = (int)v;
    }
    free(copy);
}

static Bool
x12IndexForcedSandbox(int index)
{
    int i;

    for (i = 0; i < x12ForceSandboxCount; i++) {
        if (x12ForceSandbox[i] == index)
            return TRUE;
    }
    return FALSE;
}

int
X12LevelOfClient(ClientPtr client)
{
    X12LevelStateRec *st;

    if (!client)
        return x12DefaultClientLevel;
    st = dixLookupPrivate(&client->devPrivates, x12LevelKey);
    if (!st)
        return x12DefaultClientLevel;
    return st->level;
}

void
X12LevelSetClient(ClientPtr client, int level)
{
    X12LevelStateRec *st;

    if (!client)
        return;
    if (level < X12_LEVEL_SANDBOX || level > X12_LEVEL_FULL)
        level = x12DefaultClientLevel;
    st = dixLookupPrivate(&client->devPrivates, x12LevelKey);
    if (st)
        st->level = level;
}

static void
X12LevelClientState(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    NewClientInfoRec *pci = calldata;
    X12LevelStateRec *st;
    int level;

    st = dixLookupPrivate(&pci->client->devPrivates, x12LevelKey);
    if (!st)
        return;

    switch (pci->client->clientState) {
    case ClientStateInitial:
        level = x12DefaultClientLevel;
        if (x12IndexForcedSandbox(pci->client->index))
            level = X12_LEVEL_SANDBOX;
        st->level = level;
        break;
    default:
        break;
    }
}

static Bool
x12IsKeyboardDevice(DeviceIntPtr dev)
{
    if (!dev)
        return FALSE;
    if (dev == inputInfo.keyboard)
        return TRUE;
    if (dev->key)
        return TRUE;
    return FALSE;
}

static void
X12LevelDevice(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceDeviceAccessRec *rec = calldata;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level == X12_LEVEL_FULL)
        return;

    if (!x12IsKeyboardDevice(rec->dev))
        return;

    /* sandbox: no read/grab/write (blocks QueryKeymap, GrabKey, XTest inject) */
    if (level == X12_LEVEL_SANDBOX) {
        if (rec->access_mode &
            (DixReadAccess | DixGrabAccess | DixWriteAccess)) {
            rec->status = BadAccess;
        }
        return;
    }

    /* user: allow QueryKeymap (read); still block grab + inject */
    if (rec->access_mode & (DixGrabAccess | DixWriteAccess))
        rec->status = BadAccess;
}

static void
X12LevelReceive(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceReceiveAccessRec *rec = calldata;
    ClientPtr owner;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level > X12_LEVEL_SANDBOX)
        return;

    owner = wClient(rec->pWin);
    if (owner == rec->client)
        return;

    rec->status = BadAccess;
}

static void
X12LevelResource(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceResourceAccessRec *rec = calldata;
    int level;
    int cid;
    Mask steal;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level == X12_LEVEL_FULL)
        return;

    /* Foreign drawable content reads (GetImage) — sandbox and user. */
    steal = DixReadAccess;
    if (!(rec->access_mode & steal))
        return;
    if (!(rec->rtype & RC_DRAWABLE) && rec->rtype != X11_RESTYPE_WINDOW &&
        rec->rtype != X11_RESTYPE_PIXMAP)
        return;

    cid = CLIENT_ID(rec->id);
    if (cid == 0) {
        /* Root / server-owned drawables: deny screen scrape for sandbox+user. */
        rec->status = BadAccess;
        return;
    }
    if (clients[cid] == rec->client)
        return;

    rec->status = BadAccess;
}

static void
X12LevelSelection(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceSelectionAccessRec *rec = calldata;
    Selection *sel;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level == X12_LEVEL_FULL)
        return;

    sel = rec->ppSel ? *rec->ppSel : NULL;
    if (!sel)
        return;

    /* Creating / claiming ownership is SetAttr; allow if no prior owner or self. */
    if (sel->client == rec->client || sel->client == NULL)
        return;

    if (rec->access_mode &
        (DixReadAccess | DixGetAttrAccess | DixSetAttrAccess | DixWriteAccess)) {
        rec->status = BadAccess;
    }
}

static void
X12LevelProperty(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XacePropertyAccessRec *rec = calldata;
    ClientPtr owner;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    /* Property sniff: sandbox only (user may read WM props for toolkit use). */
    if (level > X12_LEVEL_SANDBOX)
        return;

    owner = wClient(rec->pWin);
    if (owner == rec->client)
        return;

    if (rec->access_mode &
        (DixReadAccess | DixGetAttrAccess | DixListPropAccess)) {
        rec->status = BadAccess;
    }
}

static Bool
x12IsDeniedExtension(const char *name)
{
    if (!name)
        return FALSE;
    if (!strcmp(name, "XTEST") || !strcmp(name, "XTest"))
        return TRUE;
    if (!strcmp(name, "RECORD"))
        return TRUE;
    return FALSE;
}

static void
X12LevelExtension(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceExtAccessRec *rec = calldata;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level == X12_LEVEL_FULL)
        return;

    /* sandbox + user: no input injection / RECORD snooping */
    if (rec->ext && x12IsDeniedExtension(rec->ext->name))
        rec->status = BadAccess;
}

static void
X12LevelSend(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceSendAccessRec *rec = calldata;
    ClientPtr owner;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level > X12_LEVEL_SANDBOX)
        return;

    /* Sandbox may not SendEvent to foreign windows. */
    if (!rec->pWin)
        return;
    owner = wClient(rec->pWin);
    if (owner == rec->client)
        return;
    rec->status = BadAccess;
}

static void
X12LevelResetProc(ExtensionEntry *extEntry)
{
    DeleteCallback(&ClientStateCallback, X12LevelClientState, NULL);
    XaceDeleteCallback(XACE_DEVICE_ACCESS, X12LevelDevice, NULL);
    XaceDeleteCallback(XACE_RECEIVE_ACCESS, X12LevelReceive, NULL);
    XaceDeleteCallback(XACE_RESOURCE_ACCESS, X12LevelResource, NULL);
    XaceDeleteCallback(XACE_SELECTION_ACCESS, X12LevelSelection, NULL);
    XaceDeleteCallback(XACE_PROPERTY_ACCESS, X12LevelProperty, NULL);
    XaceDeleteCallback(XACE_EXT_ACCESS, X12LevelExtension, NULL);
    XaceDeleteCallback(XACE_EXT_DISPATCH, X12LevelExtension, NULL);
    XaceDeleteCallback(XACE_SEND_ACCESS, X12LevelSend, NULL);
}

void
X12LevelExtensionInit(void)
{
    ExtensionEntry *extEntry;
    int ret = TRUE;

    if (!dixRegisterPrivateKey(x12LevelKey, PRIVATE_CLIENT,
                               sizeof(X12LevelStateRec)))
        FatalError("X12Level: cannot allocate client private\n");

    ret &= AddCallback(&ClientStateCallback, X12LevelClientState, NULL);
    ret &= XaceRegisterCallback(XACE_DEVICE_ACCESS, X12LevelDevice, NULL);
    ret &= XaceRegisterCallback(XACE_RECEIVE_ACCESS, X12LevelReceive, NULL);
    ret &= XaceRegisterCallback(XACE_RESOURCE_ACCESS, X12LevelResource, NULL);
    ret &= XaceRegisterCallback(XACE_SELECTION_ACCESS, X12LevelSelection, NULL);
    ret &= XaceRegisterCallback(XACE_PROPERTY_ACCESS, X12LevelProperty, NULL);
    ret &= XaceRegisterCallback(XACE_EXT_ACCESS, X12LevelExtension, NULL);
    ret &= XaceRegisterCallback(XACE_EXT_DISPATCH, X12LevelExtension, NULL);
    ret &= XaceRegisterCallback(XACE_SEND_ACCESS, X12LevelSend, NULL);
    if (!ret)
        FatalError("X12Level: failed to register callbacks\n");

    extEntry = AddExtension("X12-LEVEL", 0, 0,
                            ProcBadRequest, ProcBadRequest,
                            X12LevelResetProc, StandardMinorOpcode);
    if (!extEntry)
        ErrorF("X12Level: AddExtension failed\n");
    else
        ErrorF("X12Level: request matrix enabled (default=%d)\n",
               x12DefaultClientLevel);
}
