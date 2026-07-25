/* Anti-drift: xcb generated structs vs x12_proto.h size constants. */
#include <stdio.h>
#include <xcb/x12_surface.h>
#include "x12_proto.h"

#define CHECK(name, actual, expect)                                     \
    do {                                                                \
        if ((actual) != (expect)) {                                     \
            fprintf(stderr, "DRIFT %s: sizeof=%zu expect=%d\n",          \
                    (name), (size_t)(actual), (int)(expect));           \
            failed = 1;                                                 \
        } else {                                                        \
            printf("OK %s %zu\n", (name), (size_t)(actual));            \
        }                                                               \
    } while (0)

int
main(void)
{
    int failed = 0;

    CHECK("QueryVersion",
          sizeof(xcb_x12surface_query_version_request_t),
          X12_PROTO_SZ_QUERY_VERSION_REQ);
    CHECK("QueryCapabilities",
          sizeof(xcb_x12surface_query_capabilities_request_t),
          X12_PROTO_SZ_QUERY_CAPABILITIES_REQ);
    CHECK("QueryModifiers",
          sizeof(xcb_x12surface_query_modifiers_request_t),
          X12_PROTO_SZ_QUERY_MODIFIERS_REQ);
    CHECK("CreateSurface",
          sizeof(xcb_x12surface_create_surface_request_t),
          X12_PROTO_SZ_CREATE_SURFACE_REQ);
    CHECK("DestroySurface",
          sizeof(xcb_x12surface_destroy_surface_request_t),
          X12_PROTO_SZ_DESTROY_SURFACE_REQ);
    CHECK("ImportSyncobj",
          sizeof(xcb_x12surface_import_syncobj_request_t),
          X12_PROTO_SZ_IMPORT_SYNCOBJ_REQ);
    CHECK("FreeSyncobj",
          sizeof(xcb_x12surface_free_syncobj_request_t),
          X12_PROTO_SZ_FREE_SYNCOBJ_REQ);
    CHECK("Attach",
          sizeof(xcb_x12surface_attach_request_t),
          X12_PROTO_SZ_ATTACH_REQ);
    CHECK("Detach",
          sizeof(xcb_x12surface_detach_request_t),
          X12_PROTO_SZ_DETACH_REQ);
    CHECK("Present",
          sizeof(xcb_x12surface_present_request_t),
          X12_PROTO_SZ_PRESENT_REQ);
    CHECK("SelectInput",
          sizeof(xcb_x12surface_select_input_request_t),
          X12_PROTO_SZ_SELECT_INPUT_REQ);

    /* Opcode constants */
    if (XCB_X12SURFACE_QUERY_VERSION != X12_PROTO_OP_QUERY_VERSION ||
        XCB_X12SURFACE_CREATE_SURFACE != X12_PROTO_OP_CREATE_SURFACE ||
        XCB_X12SURFACE_PRESENT != X12_PROTO_OP_PRESENT ||
        XCB_X12SURFACE_IMPORT_SYNCOBJ != X12_PROTO_OP_IMPORT_SYNCOBJ) {
        fprintf(stderr, "DRIFT opcode constants\n");
        failed = 1;
    }

    return failed;
}
