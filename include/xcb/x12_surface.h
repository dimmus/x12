/*
 * This file generated automatically from x12_surface.xml by c_client.py.
 * Edit at your peril.
 */

/**
 * @defgroup XCB_X12Surface_API XCB X12Surface API
 * @brief X12Surface XCB Protocol Implementation.
 * @{
 **/

#ifndef __X12_SURFACE_H
#define __X12_SURFACE_H

#include "xcb/xcb.h"
#include "xcb/xproto.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XCB_X12SURFACE_MAJOR_VERSION 1
#define XCB_X12SURFACE_MINOR_VERSION 0

extern xcb_extension_t xcb_x12surface_id;

typedef uint32_t xcb_x12surface_surface_t;

/**
 * @brief xcb_x12surface_surface_iterator_t
 **/
typedef struct xcb_x12surface_surface_iterator_t {
    xcb_x12surface_surface_t *data;
    int                       rem;
    int                       index;
} xcb_x12surface_surface_iterator_t;

typedef uint32_t xcb_x12surface_syncobj_t;

/**
 * @brief xcb_x12surface_syncobj_iterator_t
 **/
typedef struct xcb_x12surface_syncobj_iterator_t {
    xcb_x12surface_syncobj_t *data;
    int                       rem;
    int                       index;
} xcb_x12surface_syncobj_iterator_t;

typedef uint32_t xcb_x12surface_event_t;

/**
 * @brief xcb_x12surface_event_iterator_t
 **/
typedef struct xcb_x12surface_event_iterator_t {
    xcb_x12surface_event_t *data;
    int                     rem;
    int                     index;
} xcb_x12surface_event_iterator_t;

typedef enum xcb_x12surface_capability_t {
    XCB_X12SURFACE_CAPABILITY_NONE = 0,
    XCB_X12SURFACE_CAPABILITY_SYNCOBJ = 1,
    XCB_X12SURFACE_CAPABILITY_MULTIPLANE = 2,
    XCB_X12SURFACE_CAPABILITY_MODIFIERS = 4
} xcb_x12surface_capability_t;

typedef enum xcb_x12surface_present_option_t {
    XCB_X12SURFACE_PRESENT_OPTION_NONE = 0,
    XCB_X12SURFACE_PRESENT_OPTION_ASYNC = 1,
    XCB_X12SURFACE_PRESENT_OPTION_COPY = 2,
    XCB_X12SURFACE_PRESENT_OPTION_TEAR_FREE = 4
} xcb_x12surface_present_option_t;

typedef enum xcb_x12surface_complete_mode_t {
    XCB_X12SURFACE_COMPLETE_MODE_COPY = 0,
    XCB_X12SURFACE_COMPLETE_MODE_FLIP = 1,
    XCB_X12SURFACE_COMPLETE_MODE_SKIP = 2
} xcb_x12surface_complete_mode_t;

typedef enum xcb_x12surface_event_enum_t {
    XCB_X12SURFACE_EVENT_CONFIGURE_NOTIFY = 0,
    XCB_X12SURFACE_EVENT_COMPLETE_NOTIFY = 1,
    XCB_X12SURFACE_EVENT_IDLE_NOTIFY = 2
} xcb_x12surface_event_enum_t;

typedef enum xcb_x12surface_event_mask_t {
    XCB_X12SURFACE_EVENT_MASK_NO_EVENT = 0,
    XCB_X12SURFACE_EVENT_MASK_CONFIGURE_NOTIFY = 1,
    XCB_X12SURFACE_EVENT_MASK_COMPLETE_NOTIFY = 2,
    XCB_X12SURFACE_EVENT_MASK_IDLE_NOTIFY = 4
} xcb_x12surface_event_mask_t;

typedef enum xcb_x12surface_error_t {
    XCB_X12SURFACE_ERROR_BAD_SURFACE = 0,
    XCB_X12SURFACE_ERROR_BAD_SYNCOBJ = 1
} xcb_x12surface_error_t;

/** Opcode for xcb_x12surface_surface. */
#define XCB_X12SURFACE_SURFACE 0

/**
 * @brief xcb_x12surface_surface_error_t
 **/
typedef struct xcb_x12surface_surface_error_t {
    uint8_t  response_type;
    uint8_t  error_code;
    uint16_t sequence;
    uint32_t bad_value;
    uint16_t minor_opcode;
    uint8_t  major_opcode;
} xcb_x12surface_surface_error_t;

/** Opcode for xcb_x12surface_syncobj. */
#define XCB_X12SURFACE_SYNCOBJ 1

/**
 * @brief xcb_x12surface_syncobj_error_t
 **/
typedef struct xcb_x12surface_syncobj_error_t {
    uint8_t  response_type;
    uint8_t  error_code;
    uint16_t sequence;
    uint32_t bad_value;
    uint16_t minor_opcode;
    uint8_t  major_opcode;
} xcb_x12surface_syncobj_error_t;

/**
 * @brief xcb_x12surface_query_version_cookie_t
 **/
typedef struct xcb_x12surface_query_version_cookie_t {
    unsigned int sequence;
} xcb_x12surface_query_version_cookie_t;

/** Opcode for xcb_x12surface_query_version. */
#define XCB_X12SURFACE_QUERY_VERSION 0

/**
 * @brief xcb_x12surface_query_version_request_t
 **/
typedef struct xcb_x12surface_query_version_request_t {
    uint8_t  major_opcode;
    uint8_t  minor_opcode;
    uint16_t length;
    uint32_t major_version;
    uint32_t minor_version;
} xcb_x12surface_query_version_request_t;

/**
 * @brief xcb_x12surface_query_version_reply_t
 **/
typedef struct xcb_x12surface_query_version_reply_t {
    uint8_t  response_type;
    uint8_t  pad0;
    uint16_t sequence;
    uint32_t length;
    uint32_t major_version;
    uint32_t minor_version;
} xcb_x12surface_query_version_reply_t;

/**
 * @brief xcb_x12surface_query_capabilities_cookie_t
 **/
typedef struct xcb_x12surface_query_capabilities_cookie_t {
    unsigned int sequence;
} xcb_x12surface_query_capabilities_cookie_t;

/** Opcode for xcb_x12surface_query_capabilities. */
#define XCB_X12SURFACE_QUERY_CAPABILITIES 1

/**
 * @brief xcb_x12surface_query_capabilities_request_t
 **/
typedef struct xcb_x12surface_query_capabilities_request_t {
    uint8_t        major_opcode;
    uint8_t        minor_opcode;
    uint16_t       length;
    xcb_drawable_t drawable;
} xcb_x12surface_query_capabilities_request_t;

/**
 * @brief xcb_x12surface_query_capabilities_reply_t
 **/
typedef struct xcb_x12surface_query_capabilities_reply_t {
    uint8_t  response_type;
    uint8_t  pad0;
    uint16_t sequence;
    uint32_t length;
    uint32_t capabilities;
} xcb_x12surface_query_capabilities_reply_t;

/**
 * @brief xcb_x12surface_query_modifiers_cookie_t
 **/
typedef struct xcb_x12surface_query_modifiers_cookie_t {
    unsigned int sequence;
} xcb_x12surface_query_modifiers_cookie_t;

/** Opcode for xcb_x12surface_query_modifiers. */
#define XCB_X12SURFACE_QUERY_MODIFIERS 2

/**
 * @brief xcb_x12surface_query_modifiers_request_t
 **/
typedef struct xcb_x12surface_query_modifiers_request_t {
    uint8_t        major_opcode;
    uint8_t        minor_opcode;
    uint16_t       length;
    xcb_drawable_t drawable;
    uint32_t       format;
} xcb_x12surface_query_modifiers_request_t;

/**
 * @brief xcb_x12surface_query_modifiers_reply_t
 **/
typedef struct xcb_x12surface_query_modifiers_reply_t {
    uint8_t  response_type;
    uint8_t  pad0;
    uint16_t sequence;
    uint32_t length;
    uint32_t num_window_modifiers;
    uint32_t num_screen_modifiers;
    uint8_t  pad1[16];
} xcb_x12surface_query_modifiers_reply_t;

/** Opcode for xcb_x12surface_create_surface. */
#define XCB_X12SURFACE_CREATE_SURFACE 3

/**
 * @brief xcb_x12surface_create_surface_request_t
 **/
typedef struct xcb_x12surface_create_surface_request_t {
    uint8_t                  major_opcode;
    uint8_t                  minor_opcode;
    uint16_t                 length;
    xcb_x12surface_surface_t surface;
    xcb_drawable_t           drawable;
    uint8_t                  num_buffers;
    uint8_t                  pad0[3];
    uint16_t                 width;
    uint16_t                 height;
    uint32_t                 stride0;
    uint32_t                 offset0;
    uint32_t                 stride1;
    uint32_t                 offset1;
    uint32_t                 stride2;
    uint32_t                 offset2;
    uint32_t                 stride3;
    uint32_t                 offset3;
    uint8_t                  depth;
    uint8_t                  bpp;
    uint8_t                  pad1[2];
    uint64_t                 modifier;
    uint32_t                 format;
    uint8_t                  pad2[4];
} xcb_x12surface_create_surface_request_t;

/** Opcode for xcb_x12surface_destroy_surface. */
#define XCB_X12SURFACE_DESTROY_SURFACE 4

/**
 * @brief xcb_x12surface_destroy_surface_request_t
 **/
typedef struct xcb_x12surface_destroy_surface_request_t {
    uint8_t                  major_opcode;
    uint8_t                  minor_opcode;
    uint16_t                 length;
    xcb_x12surface_surface_t surface;
} xcb_x12surface_destroy_surface_request_t;

/** Opcode for xcb_x12surface_import_syncobj. */
#define XCB_X12SURFACE_IMPORT_SYNCOBJ 5

/**
 * @brief xcb_x12surface_import_syncobj_request_t
 **/
typedef struct xcb_x12surface_import_syncobj_request_t {
    uint8_t                  major_opcode;
    uint8_t                  minor_opcode;
    uint16_t                 length;
    xcb_x12surface_syncobj_t syncobj;
    xcb_drawable_t           drawable;
} xcb_x12surface_import_syncobj_request_t;

/** Opcode for xcb_x12surface_free_syncobj. */
#define XCB_X12SURFACE_FREE_SYNCOBJ 6

/**
 * @brief xcb_x12surface_free_syncobj_request_t
 **/
typedef struct xcb_x12surface_free_syncobj_request_t {
    uint8_t                  major_opcode;
    uint8_t                  minor_opcode;
    uint16_t                 length;
    xcb_x12surface_syncobj_t syncobj;
} xcb_x12surface_free_syncobj_request_t;

/** Opcode for xcb_x12surface_attach. */
#define XCB_X12SURFACE_ATTACH 7

/**
 * @brief xcb_x12surface_attach_request_t
 **/
typedef struct xcb_x12surface_attach_request_t {
    uint8_t                  major_opcode;
    uint8_t                  minor_opcode;
    uint16_t                 length;
    xcb_window_t             window;
    xcb_x12surface_surface_t surface;
} xcb_x12surface_attach_request_t;

/** Opcode for xcb_x12surface_detach. */
#define XCB_X12SURFACE_DETACH 8

/**
 * @brief xcb_x12surface_detach_request_t
 **/
typedef struct xcb_x12surface_detach_request_t {
    uint8_t      major_opcode;
    uint8_t      minor_opcode;
    uint16_t     length;
    xcb_window_t window;
} xcb_x12surface_detach_request_t;

/** Opcode for xcb_x12surface_present. */
#define XCB_X12SURFACE_PRESENT 9

/**
 * @brief xcb_x12surface_present_request_t
 **/
typedef struct xcb_x12surface_present_request_t {
    uint8_t                  major_opcode;
    uint8_t                  minor_opcode;
    uint16_t                 length;
    xcb_window_t             window;
    xcb_x12surface_surface_t surface;
    uint32_t                 serial;
    uint8_t                  pad0[8];
    int16_t                  x_off;
    int16_t                  y_off;
    uint8_t                  pad1[4];
    xcb_x12surface_syncobj_t acquire_syncobj;
    xcb_x12surface_syncobj_t release_syncobj;
    uint64_t                 acquire_point;
    uint64_t                 release_point;
    uint32_t                 options;
    uint8_t                  pad2[4];
    uint64_t                 target_msc;
    uint64_t                 divisor;
    uint64_t                 remainder;
} xcb_x12surface_present_request_t;

/** Opcode for xcb_x12surface_select_input. */
#define XCB_X12SURFACE_SELECT_INPUT 10

/**
 * @brief xcb_x12surface_select_input_request_t
 **/
typedef struct xcb_x12surface_select_input_request_t {
    uint8_t                major_opcode;
    uint8_t                minor_opcode;
    uint16_t               length;
    xcb_x12surface_event_t eid;
    xcb_window_t           window;
    uint32_t               event_mask;
} xcb_x12surface_select_input_request_t;

/** Opcode for xcb_x12surface_configure_notify. */
#define XCB_X12SURFACE_CONFIGURE_NOTIFY 0

/**
 * @brief xcb_x12surface_configure_notify_event_t
 **/
typedef struct xcb_x12surface_configure_notify_event_t {
    uint8_t                response_type;
    uint8_t                extension;
    uint16_t               sequence;
    uint32_t               length;
    uint16_t               event_type;
    uint8_t                pad0[2];
    xcb_x12surface_event_t event;
    xcb_window_t           window;
    uint16_t               width;
    uint16_t               height;
    uint8_t                pad1[16];
} xcb_x12surface_configure_notify_event_t;

/** Opcode for xcb_x12surface_complete_notify. */
#define XCB_X12SURFACE_COMPLETE_NOTIFY 1

/**
 * @brief xcb_x12surface_complete_notify_event_t
 **/
typedef struct xcb_x12surface_complete_notify_event_t {
    uint8_t                  response_type;
    uint8_t                  extension;
    uint16_t                 sequence;
    uint32_t                 length;
    uint16_t                 event_type;
    uint8_t                  mode;
    uint8_t                  pad0;
    xcb_x12surface_event_t   event;
    xcb_window_t             window;
    uint32_t                 serial;
    uint64_t                 ust;
    uint32_t                 full_sequence;
    uint64_t                 msc;
    xcb_x12surface_surface_t surface;
    uint8_t                  pad1[4];
} XCB_PACKED xcb_x12surface_complete_notify_event_t;

/** Opcode for xcb_x12surface_idle_notify. */
#define XCB_X12SURFACE_IDLE_NOTIFY 2

/**
 * @brief xcb_x12surface_idle_notify_event_t
 **/
typedef struct xcb_x12surface_idle_notify_event_t {
    uint8_t                  response_type;
    uint8_t                  extension;
    uint16_t                 sequence;
    uint32_t                 length;
    uint16_t                 event_type;
    uint8_t                  pad0[2];
    xcb_x12surface_event_t   event;
    xcb_window_t             window;
    xcb_x12surface_surface_t surface;
    uint32_t                 serial;
    uint8_t                  pad1[4];
    uint32_t                 full_sequence;
} xcb_x12surface_idle_notify_event_t;

/**
 * Get the next element of the iterator
 * @param i Pointer to a xcb_x12surface_surface_iterator_t
 *
 * Get the next element in the iterator. The member rem is
 * decreased by one. The member data points to the next
 * element. The member index is increased by sizeof(xcb_x12surface_surface_t)
 */
void
xcb_x12surface_surface_next (xcb_x12surface_surface_iterator_t *i);

/**
 * Return the iterator pointing to the last element
 * @param i An xcb_x12surface_surface_iterator_t
 * @return  The iterator pointing to the last element
 *
 * Set the current element in the iterator to the last element.
 * The member rem is set to 0. The member data points to the
 * last element.
 */
xcb_generic_iterator_t
xcb_x12surface_surface_end (xcb_x12surface_surface_iterator_t i);

/**
 * Get the next element of the iterator
 * @param i Pointer to a xcb_x12surface_syncobj_iterator_t
 *
 * Get the next element in the iterator. The member rem is
 * decreased by one. The member data points to the next
 * element. The member index is increased by sizeof(xcb_x12surface_syncobj_t)
 */
void
xcb_x12surface_syncobj_next (xcb_x12surface_syncobj_iterator_t *i);

/**
 * Return the iterator pointing to the last element
 * @param i An xcb_x12surface_syncobj_iterator_t
 * @return  The iterator pointing to the last element
 *
 * Set the current element in the iterator to the last element.
 * The member rem is set to 0. The member data points to the
 * last element.
 */
xcb_generic_iterator_t
xcb_x12surface_syncobj_end (xcb_x12surface_syncobj_iterator_t i);

/**
 * Get the next element of the iterator
 * @param i Pointer to a xcb_x12surface_event_iterator_t
 *
 * Get the next element in the iterator. The member rem is
 * decreased by one. The member data points to the next
 * element. The member index is increased by sizeof(xcb_x12surface_event_t)
 */
void
xcb_x12surface_event_next (xcb_x12surface_event_iterator_t *i);

/**
 * Return the iterator pointing to the last element
 * @param i An xcb_x12surface_event_iterator_t
 * @return  The iterator pointing to the last element
 *
 * Set the current element in the iterator to the last element.
 * The member rem is set to 0. The member data points to the
 * last element.
 */
xcb_generic_iterator_t
xcb_x12surface_event_end (xcb_x12surface_event_iterator_t i);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_x12surface_query_version_cookie_t
xcb_x12surface_query_version (xcb_connection_t *c,
                              uint32_t          major_version,
                              uint32_t          minor_version);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will cause
 * a reply to be generated. Any returned error will be
 * placed in the event queue.
 */
xcb_x12surface_query_version_cookie_t
xcb_x12surface_query_version_unchecked (xcb_connection_t *c,
                                        uint32_t          major_version,
                                        uint32_t          minor_version);

/**
 * Return the reply
 * @param c      The connection
 * @param cookie The cookie
 * @param e      The xcb_generic_error_t supplied
 *
 * Returns the reply of the request asked by
 *
 * The parameter @p e supplied to this function must be NULL if
 * xcb_x12surface_query_version_unchecked(). is used.
 * Otherwise, it stores the error if any.
 *
 * The returned value must be freed by the caller using free().
 */
xcb_x12surface_query_version_reply_t *
xcb_x12surface_query_version_reply (xcb_connection_t                       *c,
                                    xcb_x12surface_query_version_cookie_t   cookie  /**< */,
                                    xcb_generic_error_t                   **e);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_x12surface_query_capabilities_cookie_t
xcb_x12surface_query_capabilities (xcb_connection_t *c,
                                   xcb_drawable_t    drawable);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will cause
 * a reply to be generated. Any returned error will be
 * placed in the event queue.
 */
xcb_x12surface_query_capabilities_cookie_t
xcb_x12surface_query_capabilities_unchecked (xcb_connection_t *c,
                                             xcb_drawable_t    drawable);

/**
 * Return the reply
 * @param c      The connection
 * @param cookie The cookie
 * @param e      The xcb_generic_error_t supplied
 *
 * Returns the reply of the request asked by
 *
 * The parameter @p e supplied to this function must be NULL if
 * xcb_x12surface_query_capabilities_unchecked(). is used.
 * Otherwise, it stores the error if any.
 *
 * The returned value must be freed by the caller using free().
 */
xcb_x12surface_query_capabilities_reply_t *
xcb_x12surface_query_capabilities_reply (xcb_connection_t                            *c,
                                         xcb_x12surface_query_capabilities_cookie_t   cookie  /**< */,
                                         xcb_generic_error_t                        **e);

int
xcb_x12surface_query_modifiers_sizeof (const void  *_buffer);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_x12surface_query_modifiers_cookie_t
xcb_x12surface_query_modifiers (xcb_connection_t *c,
                                xcb_drawable_t    drawable,
                                uint32_t          format);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will cause
 * a reply to be generated. Any returned error will be
 * placed in the event queue.
 */
xcb_x12surface_query_modifiers_cookie_t
xcb_x12surface_query_modifiers_unchecked (xcb_connection_t *c,
                                          xcb_drawable_t    drawable,
                                          uint32_t          format);

uint64_t *
xcb_x12surface_query_modifiers_window_modifiers (const xcb_x12surface_query_modifiers_reply_t *R);

int
xcb_x12surface_query_modifiers_window_modifiers_length (const xcb_x12surface_query_modifiers_reply_t *R);

xcb_generic_iterator_t
xcb_x12surface_query_modifiers_window_modifiers_end (const xcb_x12surface_query_modifiers_reply_t *R);

uint64_t *
xcb_x12surface_query_modifiers_screen_modifiers (const xcb_x12surface_query_modifiers_reply_t *R);

int
xcb_x12surface_query_modifiers_screen_modifiers_length (const xcb_x12surface_query_modifiers_reply_t *R);

xcb_generic_iterator_t
xcb_x12surface_query_modifiers_screen_modifiers_end (const xcb_x12surface_query_modifiers_reply_t *R);

/**
 * Return the reply
 * @param c      The connection
 * @param cookie The cookie
 * @param e      The xcb_generic_error_t supplied
 *
 * Returns the reply of the request asked by
 *
 * The parameter @p e supplied to this function must be NULL if
 * xcb_x12surface_query_modifiers_unchecked(). is used.
 * Otherwise, it stores the error if any.
 *
 * The returned value must be freed by the caller using free().
 */
xcb_x12surface_query_modifiers_reply_t *
xcb_x12surface_query_modifiers_reply (xcb_connection_t                         *c,
                                      xcb_x12surface_query_modifiers_cookie_t   cookie  /**< */,
                                      xcb_generic_error_t                     **e);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
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
                                       const int32_t            *buffers);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
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
                               const int32_t            *buffers);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
xcb_void_cookie_t
xcb_x12surface_destroy_surface_checked (xcb_connection_t         *c,
                                        xcb_x12surface_surface_t  surface);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_void_cookie_t
xcb_x12surface_destroy_surface (xcb_connection_t         *c,
                                xcb_x12surface_surface_t  surface);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
xcb_void_cookie_t
xcb_x12surface_import_syncobj_checked (xcb_connection_t         *c,
                                       xcb_x12surface_syncobj_t  syncobj,
                                       xcb_drawable_t            drawable,
                                       int32_t                   syncobj_fd);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_void_cookie_t
xcb_x12surface_import_syncobj (xcb_connection_t         *c,
                               xcb_x12surface_syncobj_t  syncobj,
                               xcb_drawable_t            drawable,
                               int32_t                   syncobj_fd);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
xcb_void_cookie_t
xcb_x12surface_free_syncobj_checked (xcb_connection_t         *c,
                                     xcb_x12surface_syncobj_t  syncobj);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_void_cookie_t
xcb_x12surface_free_syncobj (xcb_connection_t         *c,
                             xcb_x12surface_syncobj_t  syncobj);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
xcb_void_cookie_t
xcb_x12surface_attach_checked (xcb_connection_t         *c,
                               xcb_window_t              window,
                               xcb_x12surface_surface_t  surface);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_void_cookie_t
xcb_x12surface_attach (xcb_connection_t         *c,
                       xcb_window_t              window,
                       xcb_x12surface_surface_t  surface);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
xcb_void_cookie_t
xcb_x12surface_detach_checked (xcb_connection_t *c,
                               xcb_window_t      window);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_void_cookie_t
xcb_x12surface_detach (xcb_connection_t *c,
                       xcb_window_t      window);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
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
                                uint64_t                  remainder);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
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
                        uint64_t                  remainder);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 * This form can be used only if the request will not cause
 * a reply to be generated. Any returned error will be
 * saved for handling by xcb_request_check().
 */
xcb_void_cookie_t
xcb_x12surface_select_input_checked (xcb_connection_t       *c,
                                     xcb_x12surface_event_t  eid,
                                     xcb_window_t            window,
                                     uint32_t                event_mask);

/**
 *
 * @param c The connection
 * @return A cookie
 *
 * Delivers a request to the X server.
 *
 */
xcb_void_cookie_t
xcb_x12surface_select_input (xcb_connection_t       *c,
                             xcb_x12surface_event_t  eid,
                             xcb_window_t            window,
                             uint32_t                event_mask);


#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
