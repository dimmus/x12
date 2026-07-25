/* SPDX-License-Identifier: MIT */
/*
 * C ABI for the memory-safe X12 protocol front-end (safe/x12-proto).
 * Spike — not yet linked into the X server request loop.
 */
#ifndef X12_PROTO_H
#define X12_PROTO_H

#include <stddef.h>
#include <stdint.h>

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

/* ClientLevel: 0=sandbox, 1=user, 2=full */
int32_t x12_proto_decode_surface(uint8_t major_opcode,
                                 const uint8_t *buf,
                                 size_t len,
                                 uint32_t fds_attached,
                                 uint8_t level,
                                 uint8_t *out_minor);

const char *x12_proto_version(void);

#ifdef __cplusplus
}
#endif

#endif /* X12_PROTO_H */
