/* X12-SCALE QueryVersion / GetScale / OptIn smoke (raw xcb; no xcb-scale yet). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

#define X12SCALE_NAME "X12-SCALE"
#define X12_SCALE_UNITY 0x10000u

static int
fail(const char *msg)
{
    fprintf(stderr, "scale_stub: %s\n", msg);
    return 1;
}

int
main(void)
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    xcb_query_extension_reply_t *qr;
    xcb_generic_error_t *err = NULL;
    uint8_t major;
    uint8_t buf[16];
    struct iovec vec;
    xcb_protocol_request_t req = { .count = 1, .ext = NULL, .isvoid = 0 };

    c = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(c))
        return fail("connect failed");

    qr = xcb_query_extension_reply(
        c, xcb_query_extension(c, (uint16_t)strlen(X12SCALE_NAME), X12SCALE_NAME),
        NULL);
    if (!qr || !qr->present) {
        free(qr);
        return fail("X12-SCALE missing");
    }
    major = qr->major_opcode;
    free(qr);

    screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    win = xcb_generate_id(c);
    xcb_create_window(c, 24, win, screen->root, 0, 0, 16, 16, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0,
                      NULL);

    /* QueryVersion */
    req.opcode = major;
    req.isvoid = 0;
    memset(buf, 0, sizeof(buf));
    buf[0] = major;
    buf[1] = 0;
    buf[2] = 3;
    buf[3] = 0;
    buf[4] = 1; /* client major */
    vec.iov_base = buf;
    vec.iov_len = 12;
    {
        unsigned seq = xcb_send_request(c, 0, &vec, &req);
        xcb_generic_reply_t *rep = xcb_wait_for_reply(c, seq, &err);
        uint32_t *fields;
        if (err || !rep) {
            free(err);
            free(rep);
            return fail("QueryVersion failed");
        }
        fields = (uint32_t *)((uint8_t *)rep + 8);
        printf("scale_stub: X12-SCALE %u.%u\n", fields[0], fields[1]);
        if (fields[0] != 1 || fields[1] != 0) {
            free(rep);
            return fail("unexpected version");
        }
        free(rep);
    }

    /* GetScale */
    memset(buf, 0, sizeof(buf));
    buf[0] = major;
    buf[1] = 1;
    buf[2] = 2;
    buf[3] = 0;
    memcpy(buf + 4, &win, 4);
    vec.iov_base = buf;
    vec.iov_len = 8;
    {
        unsigned seq = xcb_send_request(c, 0, &vec, &req);
        xcb_generic_reply_t *rep = xcb_wait_for_reply(c, seq, &err);
        uint32_t *fields;
        if (err || !rep) {
            free(err);
            free(rep);
            return fail("GetScale failed");
        }
        fields = (uint32_t *)((uint8_t *)rep + 8);
        if (fields[0] != X12_SCALE_UNITY || fields[1] != X12_SCALE_UNITY) {
            fprintf(stderr, "scale_stub: scale %u,%u\n", fields[0], fields[1]);
            free(rep);
            return fail("expected unity scale");
        }
        printf("scale_stub: GetScale unity OK\n");
        free(rep);
    }

    /* OptIn */
    req.isvoid = 1;
    memset(buf, 0, sizeof(buf));
    buf[0] = major;
    buf[1] = 2;
    buf[2] = 3;
    buf[3] = 0;
    memcpy(buf + 4, &win, 4);
    buf[8] = 1;
    vec.iov_base = buf;
    vec.iov_len = 12;
    {
        xcb_void_cookie_t ck;
        ck.sequence =
            xcb_send_request(c, XCB_REQUEST_CHECKED, &vec, &req);
        err = xcb_request_check(c, ck);
        if (err) {
            fprintf(stderr, "scale_stub: OptIn error %u\n", err->error_code);
            free(err);
            return fail("OptIn failed");
        }
    }

    xcb_destroy_window(c, win);
    xcb_flush(c);
    xcb_disconnect(c);
    printf("scale_stub: PASSED\n");
    return 0;
}
