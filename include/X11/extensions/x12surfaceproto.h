/* SPDX-License-Identifier: MIT */
/*
 * X12-SURFACE wire protocol (server view).
 * IDL: proto/xcb/src/x12_surface.xml (ADR-0010 / ADR-0012 / ADR-0013).
 */
#ifndef _X12SURFACEPROTO_H_
#define _X12SURFACEPROTO_H_

#include <X11/Xmd.h>
#include <X11/Xproto.h>

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

/* Capability bits (match XML / x12_proto) */
#define X12SurfaceCapabilitySyncobj 1
#define X12SurfaceCapabilityMultiplane 2
#define X12SurfaceCapabilityModifiers 4

#define X12SurfaceCompleteModeCopy 0
#define X12SurfaceCompleteModeFlip 1
#define X12SurfaceCompleteModeSkip 2

#define X12SurfaceNotifyConfigure 0
#define X12SurfaceNotifyComplete 1
#define X12SurfaceNotifyIdle 2

#define X12SurfaceEventMaskConfigureNotify 1
#define X12SurfaceEventMaskCompleteNotify 2
#define X12SurfaceEventMaskIdleNotify 4

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

typedef struct
{
    BYTE type;
    BYTE pad1;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD32 capabilities;
    CARD32 pad2;
    CARD32 pad3;
    CARD32 pad4;
    CARD32 pad5;
    CARD32 pad6;
} xX12SurfaceQueryCapabilitiesReply;

#define sz_xX12SurfaceQueryCapabilitiesReply 32

typedef struct
{
    BYTE type;
    BYTE pad1;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD32 numWindowModifiers;
    CARD32 numScreenModifiers;
    CARD32 pad2;
    CARD32 pad3;
    CARD32 pad4;
    CARD32 pad5;
} xX12SurfaceQueryModifiersReply;

#define sz_xX12SurfaceQueryModifiersReply 32

/* XGE events */
typedef struct
{
    CARD8 type; /* GenericEvent */
    CARD8 extension;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD16 evtype;
    CARD8 mode;
    CARD8 pad0;
    CARD32 eid;
    CARD32 window;
    CARD32 serial;
    CARD64 ust;
    CARD64 msc;
    CARD32 surface;
    CARD32 pad1;
} xX12SurfaceCompleteNotify;

#define sz_xX12SurfaceCompleteNotify 48

typedef struct
{
    CARD8 type;
    CARD8 extension;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD16 evtype;
    CARD16 pad2;
    CARD32 eid;
    CARD32 window;
    CARD32 surface;
    CARD32 serial;
    CARD32 pad1;
} xX12SurfaceIdleNotify;

#define sz_xX12SurfaceIdleNotify 32

#endif /* _X12SURFACEPROTO_H_ */
