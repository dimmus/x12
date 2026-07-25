/* Smoke: X12-SURFACE QueryVersion against a live display. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcb/xcb.h>
#include <xcb/x12_surface.h>

int
main(void)
{
    xcb_connection_t *c;
    xcb_query_extension_cookie_t qe;
    xcb_query_extension_reply_t *qr;
    xcb_x12surface_query_version_cookie_t vc;
    xcb_x12surface_query_version_reply_t *vr;
    const char *name = "X12-SURFACE";

    c = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(c)) {
        fprintf(stderr, "query_version: cannot connect\n");
        return 1;
    }

    qe = xcb_query_extension(c, (uint16_t)strlen(name), name);
    qr = xcb_query_extension_reply(c, qe, NULL);
    if (!qr || !qr->present) {
        fprintf(stderr, "query_version: X12-SURFACE not present on server\n");
        free(qr);
        xcb_disconnect(c);
        return 2;
    }
    free(qr);

    /* Ensure xcb extension table knows the major opcode. */
    (void)xcb_get_extension_data(c, &xcb_x12surface_id);

    vc = xcb_x12surface_query_version(c, 1, 0);
    vr = xcb_x12surface_query_version_reply(c, vc, NULL);
    if (!vr) {
        fprintf(stderr, "query_version: reply failed\n");
        xcb_disconnect(c);
        return 3;
    }

    printf("X12-SURFACE %u.%u\n", vr->major_version, vr->minor_version);
    if (vr->major_version != 1 || vr->minor_version != 0) {
        fprintf(stderr, "query_version: unexpected version\n");
        free(vr);
        xcb_disconnect(c);
        return 4;
    }

    free(vr);
    xcb_disconnect(c);
    return 0;
}
