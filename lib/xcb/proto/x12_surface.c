/*
 * This file generated automatically from x12_surface.xml by c_client.py.
 * Edit at your peril.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>  /* for offsetof() */
#include "xcb/xcbext.h"
#include "x12_surface.h"

#define ALIGNOF(type) offsetof(struct { char dummy; type member; }, member)
#include "xcb/xproto.h"

xcb_extension_t xcb_x12surface_id = { "X12-SURFACE", 0 };

void
xcb_x12surface_surface_next (xcb_x12surface_surface_iterator_t *i)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_x12surface_surface_t);
}

xcb_generic_iterator_t
xcb_x12surface_surface_end (xcb_x12surface_surface_iterator_t i)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}

void
xcb_x12surface_syncobj_next (xcb_x12surface_syncobj_iterator_t *i)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_x12surface_syncobj_t);
}

xcb_generic_iterator_t
xcb_x12surface_syncobj_end (xcb_x12surface_syncobj_iterator_t i)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}

void
xcb_x12surface_event_next (xcb_x12surface_event_iterator_t *i)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_x12surface_event_t);
}

xcb_generic_iterator_t
xcb_x12surface_event_end (xcb_x12surface_event_iterator_t i)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}

xcb_x12surface_query_version_cookie_t
xcb_x12surface_query_version (xcb_connection_t *c,
                              uint32_t          major_version,
                              uint32_t          minor_version)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_QUERY_VERSION,
        .isvoid = 0
    };

    struct iovec xcb_parts[4];
    xcb_x12surface_query_version_cookie_t xcb_ret;
    xcb_x12surface_query_version_request_t xcb_out;

    xcb_out.major_version = major_version;
    xcb_out.minor_version = minor_version;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_x12surface_query_version_cookie_t
xcb_x12surface_query_version_unchecked (xcb_connection_t *c,
                                        uint32_t          major_version,
                                        uint32_t          minor_version)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_QUERY_VERSION,
        .isvoid = 0
    };

    struct iovec xcb_parts[4];
    xcb_x12surface_query_version_cookie_t xcb_ret;
    xcb_x12surface_query_version_request_t xcb_out;

    xcb_out.major_version = major_version;
    xcb_out.minor_version = minor_version;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_x12surface_query_version_reply_t *
xcb_x12surface_query_version_reply (xcb_connection_t                       *c,
                                    xcb_x12surface_query_version_cookie_t   cookie  /**< */,
                                    xcb_generic_error_t                   **e)
{
    return (xcb_x12surface_query_version_reply_t *) xcb_wait_for_reply(c, cookie.sequence, e);
}

xcb_x12surface_query_capabilities_cookie_t
xcb_x12surface_query_capabilities (xcb_connection_t *c,
                                   xcb_drawable_t    drawable)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_QUERY_CAPABILITIES,
        .isvoid = 0
    };

    struct iovec xcb_parts[4];
    xcb_x12surface_query_capabilities_cookie_t xcb_ret;
    xcb_x12surface_query_capabilities_request_t xcb_out;

    xcb_out.drawable = drawable;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_x12surface_query_capabilities_cookie_t
xcb_x12surface_query_capabilities_unchecked (xcb_connection_t *c,
                                             xcb_drawable_t    drawable)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_QUERY_CAPABILITIES,
        .isvoid = 0
    };

    struct iovec xcb_parts[4];
    xcb_x12surface_query_capabilities_cookie_t xcb_ret;
    xcb_x12surface_query_capabilities_request_t xcb_out;

    xcb_out.drawable = drawable;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_x12surface_query_capabilities_reply_t *
xcb_x12surface_query_capabilities_reply (xcb_connection_t                            *c,
                                         xcb_x12surface_query_capabilities_cookie_t   cookie  /**< */,
                                         xcb_generic_error_t                        **e)
{
    return (xcb_x12surface_query_capabilities_reply_t *) xcb_wait_for_reply(c, cookie.sequence, e);
}

int
xcb_x12surface_query_modifiers_sizeof (const void  *_buffer)
{
    char *xcb_tmp = (char *)_buffer;
    const xcb_x12surface_query_modifiers_reply_t *_aux = (xcb_x12surface_query_modifiers_reply_t *)_buffer;
    unsigned int xcb_buffer_len = 0;
    unsigned int xcb_block_len = 0;
    unsigned int xcb_pad = 0;
    unsigned int xcb_align_to = 0;


    xcb_block_len += sizeof(xcb_x12surface_query_modifiers_reply_t);
    xcb_tmp += xcb_block_len;
    xcb_buffer_len += xcb_block_len;
    xcb_block_len = 0;
    /* window_modifiers */
    xcb_block_len += _aux->num_window_modifiers * sizeof(uint64_t);
    xcb_tmp += xcb_block_len;
    xcb_align_to = ALIGNOF(uint64_t);
    /* insert padding */
    xcb_pad = -xcb_block_len & (xcb_align_to - 1);
    xcb_buffer_len += xcb_block_len + xcb_pad;
    if (0 != xcb_pad) {
        xcb_tmp += xcb_pad;
        xcb_pad = 0;
    }
    xcb_block_len = 0;
    /* screen_modifiers */
    xcb_block_len += _aux->num_screen_modifiers * sizeof(uint64_t);
    xcb_tmp += xcb_block_len;
    xcb_align_to = ALIGNOF(uint64_t);
    /* insert padding */
    xcb_pad = -xcb_block_len & (xcb_align_to - 1);
    xcb_buffer_len += xcb_block_len + xcb_pad;
    if (0 != xcb_pad) {
        xcb_tmp += xcb_pad;
        xcb_pad = 0;
    }
    xcb_block_len = 0;

    return xcb_buffer_len;
}

xcb_x12surface_query_modifiers_cookie_t
xcb_x12surface_query_modifiers (xcb_connection_t *c,
                                xcb_drawable_t    drawable,
                                uint32_t          format)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_QUERY_MODIFIERS,
        .isvoid = 0
    };

    struct iovec xcb_parts[4];
    xcb_x12surface_query_modifiers_cookie_t xcb_ret;
    xcb_x12surface_query_modifiers_request_t xcb_out;

    xcb_out.drawable = drawable;
    xcb_out.format = format;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_x12surface_query_modifiers_cookie_t
xcb_x12surface_query_modifiers_unchecked (xcb_connection_t *c,
                                          xcb_drawable_t    drawable,
                                          uint32_t          format)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_QUERY_MODIFIERS,
        .isvoid = 0
    };

    struct iovec xcb_parts[4];
    xcb_x12surface_query_modifiers_cookie_t xcb_ret;
    xcb_x12surface_query_modifiers_request_t xcb_out;

    xcb_out.drawable = drawable;
    xcb_out.format = format;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

uint64_t *
xcb_x12surface_query_modifiers_window_modifiers (const xcb_x12surface_query_modifiers_reply_t *R)
{
    return (uint64_t *) (R + 1);
}

int
xcb_x12surface_query_modifiers_window_modifiers_length (const xcb_x12surface_query_modifiers_reply_t *R)
{
    return R->num_window_modifiers;
}

xcb_generic_iterator_t
xcb_x12surface_query_modifiers_window_modifiers_end (const xcb_x12surface_query_modifiers_reply_t *R)
{
    xcb_generic_iterator_t i;
    i.data = ((uint64_t *) (R + 1)) + (R->num_window_modifiers);
    i.rem = 0;
    i.index = (char *) i.data - (char *) R;
    return i;
}

uint64_t *
xcb_x12surface_query_modifiers_screen_modifiers (const xcb_x12surface_query_modifiers_reply_t *R)
{
    xcb_generic_iterator_t prev = xcb_x12surface_query_modifiers_window_modifiers_end(R);
    return (uint64_t *) ((char *) prev.data + XCB_TYPE_PAD(uint64_t, prev.index) + 0);
}

int
xcb_x12surface_query_modifiers_screen_modifiers_length (const xcb_x12surface_query_modifiers_reply_t *R)
{
    return R->num_screen_modifiers;
}

xcb_generic_iterator_t
xcb_x12surface_query_modifiers_screen_modifiers_end (const xcb_x12surface_query_modifiers_reply_t *R)
{
    xcb_generic_iterator_t i;
    xcb_generic_iterator_t prev = xcb_x12surface_query_modifiers_window_modifiers_end(R);
    i.data = ((uint64_t *) ((char*) prev.data + XCB_TYPE_PAD(uint64_t, prev.index))) + (R->num_screen_modifiers);
    i.rem = 0;
    i.index = (char *) i.data - (char *) R;
    return i;
}

xcb_x12surface_query_modifiers_reply_t *
xcb_x12surface_query_modifiers_reply (xcb_connection_t                         *c,
                                      xcb_x12surface_query_modifiers_cookie_t   cookie  /**< */,
                                      xcb_generic_error_t                     **e)
{
    return (xcb_x12surface_query_modifiers_reply_t *) xcb_wait_for_reply(c, cookie.sequence, e);
}

xcb_void_cookie_t
xcb_x12surface_create_surface_checked (xcb_connection_t         *c,
                                       xcb_x12surface_surface_t  surface,
                                       xcb_drawable_t            drawable,
                                       uint8_t                   num_buffers,
                                       uint16_t                  width,
                                       uint16_t                  height,
                                       uint32_t                  stride0,
                                       uint32_t                  offset0,
                                       uint32_t                  stride1,
                                       uint32_t                  offset1,
                                       uint32_t                  stride2,
                                       uint32_t                  offset2,
                                       uint32_t                  stride3,
                                       uint32_t                  offset3,
                                       uint8_t                   depth,
                                       uint8_t                   bpp,
                                       uint64_t                  modifier,
                                       uint32_t                  format,
                                       const int32_t            *buffers)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_CREATE_SURFACE,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_create_surface_request_t xcb_out;
    unsigned int i;
    int fds[num_buffers];
    int fd_index = 0;

    xcb_out.surface = surface;
    xcb_out.drawable = drawable;
    xcb_out.num_buffers = num_buffers;
    memset(xcb_out.pad0, 0, 3);
    xcb_out.width = width;
    xcb_out.height = height;
    xcb_out.stride0 = stride0;
    xcb_out.offset0 = offset0;
    xcb_out.stride1 = stride1;
    xcb_out.offset1 = offset1;
    xcb_out.stride2 = stride2;
    xcb_out.offset2 = offset2;
    xcb_out.stride3 = stride3;
    xcb_out.offset3 = offset3;
    xcb_out.depth = depth;
    xcb_out.bpp = bpp;
    memset(xcb_out.pad1, 0, 2);
    xcb_out.modifier = modifier;
    xcb_out.format = format;
    memset(xcb_out.pad2, 0, 4);

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    for (i = 0; i < num_buffers; i++)
        fds[fd_index++] = buffers[i];
    xcb_ret.sequence = xcb_send_request_with_fds(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req, num_buffers, fds);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_create_surface (xcb_connection_t         *c,
                               xcb_x12surface_surface_t  surface,
                               xcb_drawable_t            drawable,
                               uint8_t                   num_buffers,
                               uint16_t                  width,
                               uint16_t                  height,
                               uint32_t                  stride0,
                               uint32_t                  offset0,
                               uint32_t                  stride1,
                               uint32_t                  offset1,
                               uint32_t                  stride2,
                               uint32_t                  offset2,
                               uint32_t                  stride3,
                               uint32_t                  offset3,
                               uint8_t                   depth,
                               uint8_t                   bpp,
                               uint64_t                  modifier,
                               uint32_t                  format,
                               const int32_t            *buffers)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_CREATE_SURFACE,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_create_surface_request_t xcb_out;
    unsigned int i;
    int fds[num_buffers];
    int fd_index = 0;

    xcb_out.surface = surface;
    xcb_out.drawable = drawable;
    xcb_out.num_buffers = num_buffers;
    memset(xcb_out.pad0, 0, 3);
    xcb_out.width = width;
    xcb_out.height = height;
    xcb_out.stride0 = stride0;
    xcb_out.offset0 = offset0;
    xcb_out.stride1 = stride1;
    xcb_out.offset1 = offset1;
    xcb_out.stride2 = stride2;
    xcb_out.offset2 = offset2;
    xcb_out.stride3 = stride3;
    xcb_out.offset3 = offset3;
    xcb_out.depth = depth;
    xcb_out.bpp = bpp;
    memset(xcb_out.pad1, 0, 2);
    xcb_out.modifier = modifier;
    xcb_out.format = format;
    memset(xcb_out.pad2, 0, 4);

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    for (i = 0; i < num_buffers; i++)
        fds[fd_index++] = buffers[i];
    xcb_ret.sequence = xcb_send_request_with_fds(c, 0, xcb_parts + 2, &xcb_req, num_buffers, fds);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_destroy_surface_checked (xcb_connection_t         *c,
                                        xcb_x12surface_surface_t  surface)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_DESTROY_SURFACE,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_destroy_surface_request_t xcb_out;

    xcb_out.surface = surface;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_destroy_surface (xcb_connection_t         *c,
                                xcb_x12surface_surface_t  surface)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_DESTROY_SURFACE,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_destroy_surface_request_t xcb_out;

    xcb_out.surface = surface;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_import_syncobj_checked (xcb_connection_t         *c,
                                       xcb_x12surface_syncobj_t  syncobj,
                                       xcb_drawable_t            drawable,
                                       int32_t                   syncobj_fd)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_IMPORT_SYNCOBJ,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_import_syncobj_request_t xcb_out;
    int fds[1];
    int fd_index = 0;

    xcb_out.syncobj = syncobj;
    xcb_out.drawable = drawable;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    fds[fd_index++] = syncobj_fd;
    xcb_ret.sequence = xcb_send_request_with_fds(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req, 1, fds);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_import_syncobj (xcb_connection_t         *c,
                               xcb_x12surface_syncobj_t  syncobj,
                               xcb_drawable_t            drawable,
                               int32_t                   syncobj_fd)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_IMPORT_SYNCOBJ,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_import_syncobj_request_t xcb_out;
    int fds[1];
    int fd_index = 0;

    xcb_out.syncobj = syncobj;
    xcb_out.drawable = drawable;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    fds[fd_index++] = syncobj_fd;
    xcb_ret.sequence = xcb_send_request_with_fds(c, 0, xcb_parts + 2, &xcb_req, 1, fds);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_free_syncobj_checked (xcb_connection_t         *c,
                                     xcb_x12surface_syncobj_t  syncobj)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_FREE_SYNCOBJ,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_free_syncobj_request_t xcb_out;

    xcb_out.syncobj = syncobj;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_free_syncobj (xcb_connection_t         *c,
                             xcb_x12surface_syncobj_t  syncobj)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_FREE_SYNCOBJ,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_free_syncobj_request_t xcb_out;

    xcb_out.syncobj = syncobj;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_attach_checked (xcb_connection_t         *c,
                               xcb_window_t              window,
                               xcb_x12surface_surface_t  surface)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_ATTACH,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_attach_request_t xcb_out;

    xcb_out.window = window;
    xcb_out.surface = surface;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_attach (xcb_connection_t         *c,
                       xcb_window_t              window,
                       xcb_x12surface_surface_t  surface)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_ATTACH,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_attach_request_t xcb_out;

    xcb_out.window = window;
    xcb_out.surface = surface;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_detach_checked (xcb_connection_t *c,
                               xcb_window_t      window)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_DETACH,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_detach_request_t xcb_out;

    xcb_out.window = window;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_detach (xcb_connection_t *c,
                       xcb_window_t      window)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_DETACH,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_detach_request_t xcb_out;

    xcb_out.window = window;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_present_checked (xcb_connection_t         *c,
                                xcb_window_t              window,
                                xcb_x12surface_surface_t  surface,
                                uint32_t                  serial,
                                int16_t                   x_off,
                                int16_t                   y_off,
                                xcb_x12surface_syncobj_t  acquire_syncobj,
                                xcb_x12surface_syncobj_t  release_syncobj,
                                uint64_t                  acquire_point,
                                uint64_t                  release_point,
                                uint32_t                  options,
                                uint64_t                  target_msc,
                                uint64_t                  divisor,
                                uint64_t                  remainder)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_PRESENT,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_present_request_t xcb_out;

    xcb_out.window = window;
    xcb_out.surface = surface;
    xcb_out.serial = serial;
    memset(xcb_out.pad0, 0, 8);
    xcb_out.x_off = x_off;
    xcb_out.y_off = y_off;
    memset(xcb_out.pad1, 0, 4);
    xcb_out.acquire_syncobj = acquire_syncobj;
    xcb_out.release_syncobj = release_syncobj;
    xcb_out.acquire_point = acquire_point;
    xcb_out.release_point = release_point;
    xcb_out.options = options;
    memset(xcb_out.pad2, 0, 4);
    xcb_out.target_msc = target_msc;
    xcb_out.divisor = divisor;
    xcb_out.remainder = remainder;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_present (xcb_connection_t         *c,
                        xcb_window_t              window,
                        xcb_x12surface_surface_t  surface,
                        uint32_t                  serial,
                        int16_t                   x_off,
                        int16_t                   y_off,
                        xcb_x12surface_syncobj_t  acquire_syncobj,
                        xcb_x12surface_syncobj_t  release_syncobj,
                        uint64_t                  acquire_point,
                        uint64_t                  release_point,
                        uint32_t                  options,
                        uint64_t                  target_msc,
                        uint64_t                  divisor,
                        uint64_t                  remainder)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_PRESENT,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_present_request_t xcb_out;

    xcb_out.window = window;
    xcb_out.surface = surface;
    xcb_out.serial = serial;
    memset(xcb_out.pad0, 0, 8);
    xcb_out.x_off = x_off;
    xcb_out.y_off = y_off;
    memset(xcb_out.pad1, 0, 4);
    xcb_out.acquire_syncobj = acquire_syncobj;
    xcb_out.release_syncobj = release_syncobj;
    xcb_out.acquire_point = acquire_point;
    xcb_out.release_point = release_point;
    xcb_out.options = options;
    memset(xcb_out.pad2, 0, 4);
    xcb_out.target_msc = target_msc;
    xcb_out.divisor = divisor;
    xcb_out.remainder = remainder;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_select_input_checked (xcb_connection_t       *c,
                                     xcb_x12surface_event_t  eid,
                                     xcb_window_t            window,
                                     uint32_t                event_mask)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_SELECT_INPUT,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_select_input_request_t xcb_out;

    xcb_out.eid = eid;
    xcb_out.window = window;
    xcb_out.event_mask = event_mask;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

xcb_void_cookie_t
xcb_x12surface_select_input (xcb_connection_t       *c,
                             xcb_x12surface_event_t  eid,
                             xcb_window_t            window,
                             uint32_t                event_mask)
{
    static const xcb_protocol_request_t xcb_req = {
        .count = 2,
        .ext = &xcb_x12surface_id,
        .opcode = XCB_X12SURFACE_SELECT_INPUT,
        .isvoid = 1
    };

    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_x12surface_select_input_request_t xcb_out;

    xcb_out.eid = eid;
    xcb_out.window = window;
    xcb_out.event_mask = event_mask;

    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;

    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

