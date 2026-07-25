/* SPDX-License-Identifier: MIT */
/*
 * X12-SCALE wire protocol (server view).
 * IDL: proto/xcb/src/x12_scale.xml (ADR-0005 / ADR-0018).
 */
#ifndef _X12SCALEPROTO_H_
#define _X12SCALEPROTO_H_

#include <X11/Xmd.h>
#include <X11/Xproto.h>

#define X12SCALE_NAME "X12-SCALE"
#define X12SCALE_MAJOR 1
#define X12SCALE_MINOR 0

#define X12SCALENumberErrors 1
#define X12SCALENumberEvents 0
#define X12SCALENumberRequests 3

#define X_X12ScaleQueryVersion 0
#define X_X12ScaleGetScale 1
#define X_X12ScaleOptIn 2

#define X12ScaleBadMonitor 0

/* 16.16 fixed-point 1.0 */
#define X12ScaleUnity 0x10000u

typedef struct
{
    CARD8 reqType;
    CARD8 x12scaleReqType;
    CARD16 length;
    CARD32 majorVersion;
    CARD32 minorVersion;
} xX12ScaleQueryVersionReq;
#define sz_xX12ScaleQueryVersionReq 12

typedef struct
{
    CARD8 type;
    CARD8 pad0;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD32 majorVersion;
    CARD32 minorVersion;
    CARD32 pad1;
    CARD32 pad2;
    CARD32 pad3;
    CARD32 pad4;
} xX12ScaleQueryVersionReply;
#define sz_xX12ScaleQueryVersionReply 32

typedef struct
{
    CARD8 reqType;
    CARD8 x12scaleReqType;
    CARD16 length;
    CARD32 window;
} xX12ScaleGetScaleReq;
#define sz_xX12ScaleGetScaleReq 8

typedef struct
{
    CARD8 type;
    CARD8 pad0;
    CARD16 sequenceNumber;
    CARD32 length;
    CARD32 scaleX;
    CARD32 scaleY;
    CARD16 widthMm;
    CARD16 heightMm;
    CARD32 pad1;
    CARD32 pad2;
    CARD32 pad3;
} xX12ScaleGetScaleReply;
#define sz_xX12ScaleGetScaleReply 32

typedef struct
{
    CARD8 reqType;
    CARD8 x12scaleReqType;
    CARD16 length;
    CARD32 window;
    BOOL enable;
    CARD8 pad0;
    CARD16 pad1;
} xX12ScaleOptInReq;
#define sz_xX12ScaleOptInReq 12

#endif /* _X12SCALEPROTO_H_ */
