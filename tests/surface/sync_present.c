/*
 * X12-SURFACE syncobj + MSC smoke (ADR-0018).
 * ImportSyncobj → Present release → Present acquire → CompleteNotify msc≠0.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <xcb/x12_surface.h>

#define WIDTH 32
#define HEIGHT 32
#define FOURCC_XR24 0x34325258u
#define MOD_LINEAR 0ull

static void
die(const char *msg)
{
    fprintf(stderr, "sync_present: %s\n", msg);
    exit(1);
}

static int
make_memfd(const uint32_t *pixels, size_t bytes)
{
    int fd = memfd_create("x12-sync-present", MFD_CLOEXEC);
    if (fd < 0)
        return -1;
    if (ftruncate(fd, (off_t)bytes) != 0) {
        close(fd);
        return -1;
    }
    void *map = mmap(NULL, bytes, PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        return -1;
    }
    memcpy(map, pixels, bytes);
    munmap(map, bytes);
    return fd;
}

static int
check_void(xcb_connection_t *c, xcb_void_cookie_t ck, const char *what)
{
    xcb_generic_error_t *err = xcb_request_check(c, ck);
    if (err) {
        fprintf(stderr, "sync_present: %s error %u major=%u minor=%u\n", what,
                err->error_code, err->major_code, err->minor_code);
        free(err);
        return -1;
    }
    return 0;
}

int
main(void)
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    xcb_x12surface_surface_t surface;
    xcb_x12surface_syncobj_t syncobj;
    xcb_x12surface_event_t eid;
    xcb_query_extension_reply_t *qr;
    xcb_generic_event_t *ev;
    uint32_t *pixels;
    size_t bytes;
    int fd, dummy_fd;
    int saw_complete = 0;
    uint64_t complete_msc = 0;
    const char *extname = "X12-SURFACE";
    int i;

    bytes = (size_t)WIDTH * HEIGHT * 4;
    pixels = calloc(WIDTH * HEIGHT, sizeof(uint32_t));
    if (!pixels)
        die("oom");
    for (i = 0; i < WIDTH * HEIGHT; i++)
        pixels[i] = 0x000000ffu;
    fd = make_memfd(pixels, bytes);
    free(pixels);
    if (fd < 0)
        die("memfd");

    dummy_fd = memfd_create("x12-syncobj-stub", MFD_CLOEXEC);
    if (dummy_fd < 0) {
        close(fd);
        die("syncobj memfd");
    }

    c = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(c))
        die("connect");

    qr = xcb_query_extension_reply(
        c, xcb_query_extension(c, (uint16_t)strlen(extname), extname), NULL);
    if (!qr || !qr->present) {
        free(qr);
        die("X12-SURFACE missing");
    }
    free(qr);
    (void)xcb_get_extension_data(c, &xcb_x12surface_id);

    screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    win = xcb_generate_id(c);
    xcb_create_window(c, 24, win, screen->root, 0, 0, WIDTH, HEIGHT, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0,
                      NULL);
    xcb_map_window(c, win);

    {
        xcb_x12surface_query_capabilities_cookie_t ck =
            xcb_x12surface_query_capabilities(c, win);
        xcb_x12surface_query_capabilities_reply_t *rep =
            xcb_x12surface_query_capabilities_reply(c, ck, NULL);
        if (!rep)
            die("QueryCapabilities");
        if (!(rep->capabilities & XCB_X12SURFACE_CAPABILITY_SYNCOBJ)) {
            free(rep);
            die("Capability.Syncobj not advertised");
        }
        printf("sync_present: capabilities=0x%x\n", rep->capabilities);
        free(rep);
    }

    surface = xcb_generate_id(c);
    {
        int32_t bufs[1] = { fd };
        if (check_void(c,
                       xcb_x12surface_create_surface_checked(
                           c, surface, win, 1, WIDTH, HEIGHT, WIDTH * 4, 0, 0,
                           0, 0, 0, 0, 0, 24, 32, MOD_LINEAR, FOURCC_XR24, bufs),
                       "CreateSurface") != 0)
            return 4;
    }
    close(fd);

    syncobj = xcb_generate_id(c);
    if (check_void(c,
                   xcb_x12surface_import_syncobj_checked(c, syncobj, win,
                                                         dummy_fd),
                   "ImportSyncobj") != 0)
        return 5;
    close(dummy_fd);

    eid = xcb_generate_id(c);
    if (check_void(c,
                   xcb_x12surface_select_input_checked(
                       c, eid, win,
                       XCB_X12SURFACE_EVENT_MASK_COMPLETE_NOTIFY |
                           XCB_X12SURFACE_EVENT_MASK_IDLE_NOTIFY),
                   "SelectInput") != 0)
        return 6;

    if (check_void(c, xcb_x12surface_attach_checked(c, win, surface),
                   "Attach") != 0)
        return 7;

    /* Frame 1: no acquire; signal release_point=1 */
    if (check_void(c,
                   xcb_x12surface_present_checked(
                       c, win, surface, 1, 0, 0, 0, syncobj, 0, 1,
                       XCB_X12SURFACE_PRESENT_OPTION_ASYNC, 0, 0, 0),
                   "Present#1") != 0)
        return 8;

    /* Frame 2: acquire_point=1 must succeed; release_point=2 */
    if (check_void(c,
                   xcb_x12surface_present_checked(
                       c, win, surface, 2, 0, 0, syncobj, syncobj, 1, 2,
                       XCB_X12SURFACE_PRESENT_OPTION_ASYNC, 0, 0, 0),
                   "Present#2") != 0)
        return 9;

    /* Frame 3: wait one fake MSC into the future */
    if (check_void(c,
                   xcb_x12surface_present_checked(
                       c, win, surface, 3, 0, 0, syncobj, syncobj, 2, 3, 0,
                       /* target_msc large enough relative to clock */ 0, 0, 0),
                   "Present#3") != 0)
        return 10;

    xcb_flush(c);

    for (i = 0; i < 200 && saw_complete < 3; i++) {
        ev = xcb_wait_for_event(c);
        if (!ev)
            break;
        if ((ev->response_type & 0x7f) == XCB_GE_GENERIC) {
            xcb_ge_generic_event_t *ge = (xcb_ge_generic_event_t *)ev;
            if (ge->event_type == XCB_X12SURFACE_COMPLETE_NOTIFY) {
                xcb_x12surface_complete_notify_event_t *cn =
                    (xcb_x12surface_complete_notify_event_t *)ev;
                complete_msc = cn->msc;
                saw_complete++;
                printf("sync_present: Complete serial=%u msc=%llu ust=%llu\n",
                       cn->serial, (unsigned long long)cn->msc,
                       (unsigned long long)cn->ust);
            }
        }
        free(ev);
    }

    if (saw_complete < 1)
        die("no CompleteNotify");
    if (complete_msc == 0 && saw_complete < 3) {
        /* At least one non-zero MSC expected from the fake clock. */
        fprintf(stderr, "sync_present: warning msc still 0 after %d completes\n",
                saw_complete);
    }
    if (complete_msc == 0)
        die("CompleteNotify msc is 0 (clock not wired)");

    if (check_void(c, xcb_x12surface_free_syncobj_checked(c, syncobj),
                   "FreeSyncobj") != 0)
        return 11;

    xcb_x12surface_destroy_surface(c, surface);
    xcb_destroy_window(c, win);
    xcb_flush(c);
    xcb_disconnect(c);
    printf("sync_present: PASSED\n");
    return 0;
}
