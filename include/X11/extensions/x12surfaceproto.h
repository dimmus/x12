/* SPDX-License-Identifier: MIT */
/*
 * X12-SURFACE wire protocol (server view).
 * IDL: proto/xcb/src/x12_surface.xml (ADR-0010 / ADR-0012).
 */
#ifndef _X12SURFACEPROTO_H_
#define _X12SURFACEPROTO_H_

#include <X11/Xmd.h>

#define X12SURFACE_NAME "X12-SURFACE"
#define X12SURFACE_MAJOR 1
#define X12SURFACE_MINOR 0

#define X12SURFACENumberErrors 2
#define X12SURFACENumberEvents 0
#define X12SURFACENumberRequests 11

#define X_X12SurfaceQueryVersion 0
#define X_X12SurfaceQueryCapabilities 1
#define X_X12SurfaceQueryModifiers 2
#define X_X12SurfaceCreateSurface 3
#define X_X12SurfaceDestroySurface 4
#define X_X12SurfaceImportSyncobj 5
#define X_X12SurfaceFreeSyncobj 6
#define X_X12SurfaceAttach 7
#define X_X12SurfaceDetach 8
#define X_X12SurfacePresent 9
#define X_X12SurfaceSelectInput 10

#define X12SurfaceBadSurface 0
#define X12SurfaceBadSyncobj 1

typedef struct
{
    CARD8 reqType;
    CARD8 x12ReqType;
    CARD16 length;
    CARD32 majorVersion;
    CARD32 minorVersion;
} xX12SurfaceQueryVersionReq;

#define sz_xX12SurfaceQueryVersionReq 12

typedef struct
{
    BYTE type; /* X_Reply */
    BYTE pad1;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD32 majorVersion;
    CARD32 minorVersion;
    CARD32 pad2;
    CARD32 pad3;
    CARD32 pad4;
    CARD32 pad5;
} xX12SurfaceQueryVersionReply;

#define sz_xX12SurfaceQueryVersionReply 32

#endif /* _X12SURFACEPROTO_H_ */
