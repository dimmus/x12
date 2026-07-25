/* SPDX-License-Identifier: MIT
 *
 * X12 hierarchical client levels via XACE (ADR-0003, G1).
 *
 * Levels: sandbox (0) < user (1) < full (2)
 * Default: full (-client-level)
 *
 * Sandbox restrictions (keylog deny for G1):
 *  - No DixReadAccess on the core keyboard (QueryKeymap scrubbed)
 *  - No DixGrabAccess on the core keyboard (no GrabKey keylogging)
 *  - No receiving events on windows owned by other clients
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

/* Optional forced sandbox client indices (testing / G1 demo). */
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

/*
 * Deny sandbox clients DixReadAccess / DixGrabAccess on the core keyboard.
 * QueryKeymap uses DixReadAccess and returns an empty map on BadAccess.
 */
static void
X12LevelDevice(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XaceDeviceAccessRec *rec = calldata;
    int level;

    if (rec->status != Success)
        return;

    level = X12LevelOfClient(rec->client);
    if (level > X12_LEVEL_SANDBOX)
        return;

    /* Only restrict the core keyboard for G1 keylog deny. */
    if (rec->dev != inputInfo.keyboard)
        return;

    if (rec->access_mode & (DixReadAccess | DixGrabAccess)) {
        ErrorF("X12Level: deny client %d keyboard access mode 0x%lx (sandbox)\n",
               rec->client->index, (unsigned long)rec->access_mode);
        rec->status = BadAccess;
    }
}

/*
 * Sandbox clients may not receive events on windows they do not own.
 * Blocks selecting KeyPress on other clients' windows / root owned by server
 * when the event is delivered to the sandbox client.
 */
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

    /* Allow sandbox to receive on its own windows only. */
    ErrorF("X12Level: deny client %d receive on window 0x%lx owned by %d (sandbox)\n",
           rec->client->index, (unsigned long)rec->pWin->drawable.id,
           owner ? owner->index : -1);
    rec->status = BadAccess;
}

static void
X12LevelResetProc(ExtensionEntry *extEntry)
{
    DeleteCallback(&ClientStateCallback, X12LevelClientState, NULL);
    XaceDeleteCallback(XACE_DEVICE_ACCESS, X12LevelDevice, NULL);
    XaceDeleteCallback(XACE_RECEIVE_ACCESS, X12LevelReceive, NULL);
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
    if (!ret)
        FatalError("X12Level: failed to register callbacks\n");

    /* Named extension (no requests) so it appears in XListExtensions. */
    extEntry = AddExtension("X12-LEVEL", 0, 0,
                            ProcBadRequest, ProcBadRequest,
                            X12LevelResetProc, StandardMinorOpcode);
    if (!extEntry)
        ErrorF("X12Level: AddExtension failed\n");
    else
        ErrorF("X12Level: hierarchical levels enabled (default=%d)\n",
               x12DefaultClientLevel);
}
