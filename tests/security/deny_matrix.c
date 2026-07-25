/*
 * Post-G1 X12-LEVEL request matrix (ADR-0016).
 *
 * Client 1 = full, client 2 = sandbox (-sandbox-clients 2).
 * Verifies: GetImage deny, property sniff deny, XTEST inject deny, keylog scrub.
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int g_xerrs;

static int
ignore_xerr(Display *d, XErrorEvent *e)
{
    (void)d;
    (void)e;
    g_xerrs++;
    return 0;
}

static int
keymap_nonzero(const char map[32])
{
    int i;
    for (i = 0; i < 32; i++)
        if (map[i])
            return 1;
    return 0;
}

static int
fail(const char *msg)
{
    fprintf(stderr, "deny_matrix: FAIL — %s\n", msg);
    return 1;
}

int
main(void)
{
    Display *full, *sand;
    Window win;
    XImage *img;
    Atom wm;
    char map_full[32], map_sand[32];
    KeyCode kc;
    int major = 0, minor = 0, evt = 0, err = 0;
    int (*old_handler)(Display *, XErrorEvent *);

    full = XOpenDisplay(NULL);
    sand = XOpenDisplay(NULL);
    if (!full || !sand)
        return fail("cannot open displays");

    if (!XTestQueryExtension(full, &evt, &err, &major, &minor))
        return fail("full missing XTEST");

    old_handler = XSetErrorHandler(ignore_xerr);

    win = XCreateSimpleWindow(full, DefaultRootWindow(full), 0, 0, 32, 32, 0,
                              BlackPixel(full, DefaultScreen(full)),
                              WhitePixel(full, DefaultScreen(full)));
    XMapWindow(full, win);
    XStoreName(full, win, "secret");
    XSync(full, False);

    /* GetImage on foreign window */
    g_xerrs = 0;
    img = XGetImage(sand, win, 0, 0, 16, 16, AllPlanes, ZPixmap);
    XSync(sand, False);
    if (img) {
        XDestroyImage(img);
        XSetErrorHandler(old_handler);
        return fail("sandbox XGetImage on foreign window succeeded");
    }
    printf("deny_matrix: GetImage denied OK\n");

    /* Foreign property read */
    {
        Atom actual = None;
        int fmt = 0;
        unsigned long n = 0, bytes = 0;
        unsigned char *data = NULL;
        wm = XInternAtom(full, "WM_NAME", False);
        g_xerrs = 0;
        if (XGetWindowProperty(sand, win, wm, 0, 64, False, AnyPropertyType,
                               &actual, &fmt, &n, &bytes, &data) == Success &&
            data && n > 0) {
            XFree(data);
            XSetErrorHandler(old_handler);
            return fail("sandbox read foreign WM_NAME");
        }
        if (data)
            XFree(data);
    }
    printf("deny_matrix: property sniff denied OK\n");

    /* Keylog scrub */
    kc = XKeysymToKeycode(full, XK_a);
    if (!kc)
        kc = 38;
    XTestFakeKeyEvent(full, kc, True, CurrentTime);
    XFlush(full);
    usleep(50000);
    memset(map_full, 0, sizeof(map_full));
    memset(map_sand, 0, sizeof(map_sand));
    XQueryKeymap(full, map_full);
    XQueryKeymap(sand, map_sand);
    XTestFakeKeyEvent(full, kc, False, CurrentTime);
    XFlush(full);
    if (!keymap_nonzero(map_full)) {
        XSetErrorHandler(old_handler);
        return fail("full QueryKeymap empty");
    }
    if (keymap_nonzero(map_sand)) {
        XSetErrorHandler(old_handler);
        return fail("sandbox saw keymap bits");
    }
    printf("deny_matrix: keylog scrub OK\n");

    /* Sandbox must not inject via XTest */
    {
        char before[32], after[32];
        memset(before, 0, sizeof(before));
        XQueryKeymap(full, before);
        g_xerrs = 0;
        XTestFakeKeyEvent(sand, kc, True, CurrentTime);
        XSync(sand, False);
        usleep(50000);
        memset(after, 0, sizeof(after));
        XQueryKeymap(full, after);
        XTestFakeKeyEvent(sand, kc, False, CurrentTime);
        XSync(sand, False);
        if (memcmp(before, after, 32) != 0) {
            XSetErrorHandler(old_handler);
            return fail("sandbox XTest inject affected keymap");
        }
    }
    printf("deny_matrix: XTest inject denied OK\n");

    XSetErrorHandler(old_handler);
    XDestroyWindow(full, win);
    XCloseDisplay(sand);
    XCloseDisplay(full);
    printf("deny_matrix: PASSED\n");
    return 0;
}
