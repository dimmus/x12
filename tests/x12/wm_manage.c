/*
 * X12 xwm smoke: prove the reference WM manages a mapped client (reparent).
 * DISPLAY must already point at a server where xwm is running.
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
fail(const char *msg)
{
    fprintf(stderr, "wm_manage: FAIL — %s\n", msg);
    return 1;
}

int
main(void)
{
    Display *dpy;
    Window root, win, parent, *children = NULL;
    unsigned n = 0;
    Atom wm_state, type = None;
    int fmt = 0;
    unsigned long nitems = 0, bytes = 0;
    unsigned char *data = NULL;
    int i;
    XSetWindowAttributes attr;

    dpy = XOpenDisplay(NULL);
    if (!dpy)
        return fail("cannot open display");

    root = DefaultRootWindow(dpy);
    attr.event_mask = StructureNotifyMask;
    win = XCreateWindow(dpy, root, 40, 40, 200, 120, 0,
                        CopyFromParent, InputOutput, CopyFromParent,
                        CWEventMask, &attr);
    XStoreName(dpy, win, "x12-wm-manage");
    XMapWindow(dpy, win);
    XSync(dpy, False);

    /* Wait for WM reparent (parent becomes decoration frame, not root). */
    for (i = 0; i < 50; i++) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
        }
        if (XQueryTree(dpy, win, &root, &parent, &children, &n)) {
            if (children) {
                XFree(children);
                children = NULL;
            }
            if (parent != None && parent != root) {
                printf("wm_manage: reparented parent=0x%lx root=0x%lx\n",
                       (unsigned long)parent, (unsigned long)root);
                break;
            }
        }
        usleep(100000);
    }

    if (parent == None || parent == root) {
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
        return fail("window not reparented (is xwm running?)");
    }

    wm_state = XInternAtom(dpy, "WM_STATE", False);
    for (i = 0; i < 30; i++) {
        if (XGetWindowProperty(dpy, win, wm_state, 0, 2, False, wm_state, &type,
                               &fmt, &nitems, &bytes, &data) == Success &&
            data && nitems >= 1) {
            printf("wm_manage: WM_STATE=%lu\n",
                   (unsigned long)((unsigned long *)data)[0]);
            XFree(data);
            data = NULL;
            break;
        }
        if (data) {
            XFree(data);
            data = NULL;
        }
        usleep(100000);
    }
    if (!type)
        fprintf(stderr, "wm_manage: warn — WM_STATE not set yet\n");

    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    printf("wm_manage: PASSED\n");
    return 0;
}
