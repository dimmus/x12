/* SPDX-License-Identifier: MIT */
/*
 * C ABI for the memory-safe X12 protocol front-end (safe/x12-proto).
 * Decoded out-structs are filled on X12_PROTO_OK for dix dispatch.
 */
#ifndef X12_PROTO_H
#define X12_PROTO_H

#include <stddef.h>
#include <stdint.h>

#include "x12_proto_generated.h"

#ifdef __cplusplus
extern "C" {
#endif

#define X12_PROTO_OK 0
#define X12_PROTO_TRUNCATED 1
#define X12_PROTO_BAD_LENGTH 2
#define X12_PROTO_MAJOR_MISMATCH 3
#define X12_PROTO_UNKNOWN_OPCODE 4
#define X12_PROTO_INVALID_FIELD 5
#define X12_PROTO_FD_MISMATCH 6
#define X12_PROTO_DENIED 7
#define X12_PROTO_BAD_LEVEL 8
#define X12_PROTO_NULL 9

/* Must match X12_LEVEL_* / Rust ClientLevel */
#define X12_PROTO_LEVEL_SANDBOX 0
#define X12_PROTO_LEVEL_USER 1
#define X12_PROTO_LEVEL_FULL 2

/* Minor opcodes — must match proto/xcb/src/x12_surface.xml */
#define X12_PROTO_OP_QUERY_VERSION 0
#define X12_PROTO_OP_QUERY_CAPABILITIES 1
#define X12_PROTO_OP_QUERY_MODIFIERS 2
#define X12_PROTO_OP_CREATE_SURFACE 3
#define X12_PROTO_OP_DESTROY_SURFACE 4
#define X12_PROTO_OP_IMPORT_SYNCOBJ 5
#define X12_PROTO_OP_FREE_SYNCOBJ 6
#define X12_PROTO_OP_ATTACH 7
#define X12_PROTO_OP_DETACH 8
#define X12_PROTO_OP_PRESENT 9
#define X12_PROTO_OP_SELECT_INPUT 10

/* Wire sizes including 4-byte X11 header (anti-drift vs xcb_*.h). */
#define X12_PROTO_SZ_QUERY_VERSION_REQ 12
#define X12_PROTO_SZ_QUERY_CAPABILITIES_REQ 8
#define X12_PROTO_SZ_QUERY_MODIFIERS_REQ 12
#define X12_PROTO_SZ_CREATE_SURFACE_REQ 72
#define X12_PROTO_SZ_DESTROY_SURFACE_REQ 8
#define X12_PROTO_SZ_IMPORT_SYNCOBJ_REQ 12
#define X12_PROTO_SZ_FREE_SYNCOBJ_REQ 8
#define X12_PROTO_SZ_ATTACH_REQ 12
#define X12_PROTO_SZ_DETACH_REQ 8
#define X12_PROTO_SZ_PRESENT_REQ 88
#define X12_PROTO_SZ_SELECT_INPUT_REQ 16

typedef struct x12_proto_query_version {
    uint32_t major_version;
    uint32_t minor_version;
} x12_proto_query_version_t;

typedef struct x12_proto_query_capabilities {
    uint32_t drawable;
} x12_proto_query_capabilities_t;

typedef struct x12_proto_query_modifiers {
    uint32_t drawable;
    uint32_t format;
} x12_proto_query_modifiers_t;

typedef struct x12_proto_create_surface {
    uint32_t surface;
    uint32_t drawable;
    uint8_t num_buffers;
    uint8_t depth;
    uint8_t bpp;
    uint8_t pad0;
    uint16_t width;
    uint16_t height;
    uint32_t strides[4];
    uint32_t offsets[4];
    uint64_t modifier;
    uint32_t format;
} x12_proto_create_surface_t;

typedef struct x12_proto_destroy_surface {
    uint32_t surface;
} x12_proto_destroy_surface_t;

typedef struct x12_proto_import_syncobj {
    uint32_t syncobj;
    uint32_t drawable;
} x12_proto_import_syncobj_t;

typedef struct x12_proto_free_syncobj {
    uint32_t syncobj;
} x12_proto_free_syncobj_t;

typedef struct x12_proto_attach {
    uint32_t window;
    uint32_t surface;
} x12_proto_attach_t;

typedef struct x12_proto_detach {
    uint32_t window;
} x12_proto_detach_t;

typedef struct x12_proto_present {
    uint32_t window;
    uint32_t surface;
    uint32_t serial;
    int16_t x_off;
    int16_t y_off;
    uint32_t acquire_syncobj;
    uint32_t release_syncobj;
    uint64_t acquire_point;
    uint64_t release_point;
    uint32_t options;
    uint64_t target_msc;
    uint64_t divisor;
    uint64_t remainder;
} x12_proto_present_t;

typedef struct x12_proto_select_input {
    uint32_t eid;
    uint32_t window;
    uint32_t event_mask;
} x12_proto_select_input_t;

typedef struct x12_proto_surface_decoded {
    uint8_t op; /* X12_PROTO_OP_* */
    uint8_t pad[3];
    union {
        x12_proto_query_version_t query_version;
        x12_proto_query_capabilities_t query_capabilities;
        x12_proto_query_modifiers_t query_modifiers;
        x12_proto_create_surface_t create_surface;
        x12_proto_destroy_surface_t destroy_surface;
        x12_proto_import_syncobj_t import_syncobj;
        x12_proto_free_syncobj_t free_syncobj;
        x12_proto_attach_t attach;
        x12_proto_detach_t detach;
        x12_proto_present_t present;
        x12_proto_select_input_t select_input;
    } u;
} x12_proto_surface_decoded_t;

/*
 * Decode + validate one X12-SURFACE request.
 * On success fills *out (if non-NULL) and returns X12_PROTO_OK.
 * level: X12_PROTO_LEVEL_*.
 */
int32_t x12_proto_decode_surface(uint8_t major_opcode,
                                 const uint8_t *buf,
                                 size_t len,
                                 uint32_t fds_attached,
                                 uint8_t level,
                                 x12_proto_surface_decoded_t *out);

const char *x12_proto_version(void);

#ifdef __cplusplus
}
#endif

#endif /* X12_PROTO_H */
