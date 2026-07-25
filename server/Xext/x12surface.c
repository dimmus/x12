/*
 * X12-SURFACE server stub (ADR-0012).
 * Decode/validate via Rust safe/x12-proto; QueryVersion implemented;
 * remaining opcodes return BadImplementation until step 7 compositor.
 */
#include <dix-config.h>

#include "dix/dix_priv.h"
#include "dixstruct_priv.h"
#include "extinit_priv.h"
#include "protocol-versions.h"

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/x12surfaceproto.h>

#include "x12_proto.h"
#ifdef XACE
#include "x12level.h"
#endif

static int x12SurfaceErrorBase;

static int
X12SurfaceMapDecodeError(int32_t rc)
{
    switch (rc) {
    case X12_PROTO_OK:
        return Success;
    case X12_PROTO_TRUNCATED:
    case X12_PROTO_BAD_LENGTH:
        return BadLength;
    case X12_PROTO_MAJOR_MISMATCH:
    case X12_PROTO_UNKNOWN_OPCODE:
    case X12_PROTO_INVALID_FIELD:
    case X12_PROTO_FD_MISMATCH:
    case X12_PROTO_NULL:
        return BadValue;
    case X12_PROTO_DENIED:
    case X12_PROTO_BAD_LEVEL:
        return BadAccess;
    default:
        return BadValue;
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

    rc = x12_proto_decode_surface((uint8_t)stuff->reqType,
                                  (const uint8_t *)client->requestBuffer,
                                  len,
                                  (uint32_t)client->req_fds,
                                  level,
                                  out);
    return X12SurfaceMapDecodeError(rc);
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
        swapl(&rep.length);
        swapl(&rep.majorVersion);
        swapl(&rep.minorVersion);
    }

    WriteToClient(client, sizeof(rep), &rep);
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
    case X_X12SurfaceQueryModifiers:
    case X_X12SurfaceCreateSurface:
    case X_X12SurfaceDestroySurface:
    case X_X12SurfaceImportSyncobj:
    case X_X12SurfaceFreeSyncobj:
    case X_X12SurfaceAttach:
    case X_X12SurfaceDetach:
    case X_X12SurfacePresent:
    case X_X12SurfaceSelectInput:
        /* Validated; compositor / resource path is ROADMAP step 7. */
        return BadImplementation;
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
        /*
         * Other opcodes: length already swapped; body field swaps for
         * CreateSurface/Present land with full step-7 handlers. Decode still
         * runs on host-endian fields for same-endian clients only until then.
         */
        return ProcX12SurfaceDispatch(client);
    }
}

void
X12SurfaceExtensionInit(void)
{
    ExtensionEntry *extEntry;

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
    ErrorF("X12Surface: extension enabled (v%d.%d, safe decode)\n",
           SERVER_X12SURFACE_MAJOR_VERSION,
           SERVER_X12SURFACE_MINOR_VERSION);
    ErrorF("X12Surface: front-end %s\n", x12_proto_version());
}
