/* SPDX-License-Identifier: MIT
 *
 * xwm — X12 reference window manager (from-scratch ICCCM engine).
 *
 * Usage: xwm [-display dpy] [-f config]
 */
#include "xwm.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int
xerror_ignore(Display *dpy, XErrorEvent *ev)
{
    (void)dpy;
    if (ev->error_code == BadWindow || ev->error_code == BadDrawable ||
        ev->error_code == BadMatch)
        return 0;
    fprintf(stderr, "xwm: X error %d major %d minor %d\n", ev->error_code,
            ev->request_code, ev->minor_code);
    return 0;
}

static int
xerror_wm_taken(Display *dpy, XErrorEvent *ev)
{
    (void)dpy;
    if (ev->error_code == BadAccess) {
        fprintf(stderr, "xwm: another window manager is already running\n");
        exit(1);
    }
    return 0;
}

static void
usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s [-display dpy] [-f config]\n", argv0);
}

static const char *
default_config(char *buf, size_t n)
{
    const char *home = getenv("HOME");
    if (home) {
        snprintf(buf, n, "%s/.xwmrc", home);
        if (access(buf, R_OK) == 0)
            return buf;
    }
    snprintf(buf, n, "%s/X11/xwm/system.xwmrc", DATADIR);
    if (access(buf, R_OK) == 0)
        return buf;
    return NULL;
}

int
main(int argc, char **argv)
{
    XwmState st;
    XEvent ev;
    XSetWindowAttributes wa;
    char cfgbuf[512];
    const char *cfg = NULL;
    const char *display = NULL;
    char **restart_argv = argv;
    int i;

    memset(&st, 0, sizeof(st));
    xwm_config_defaults(&st.cfg);

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-display") && i + 1 < argc) {
            display = argv[++i];
        } else if ((!strcmp(argv[i], "-f") || !strcmp(argv[i], "-file")) &&
                   i + 1 < argc) {
            cfg = argv[++i];
        } else if (!strcmp(argv[i], "-help") || !strcmp(argv[i], "--help")) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "xwm: unknown option %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (!cfg)
        cfg = default_config(cfgbuf, sizeof(cfgbuf));
    if (cfg) {
        st.config_path = strdup(cfg);
        if (xwm_config_load(&st, cfg) != 0)
            fprintf(stderr, "xwm: warning: could not load %s\n", cfg);
    } else {
        xwm_config_defaults(&st.cfg);
    }

    signal(SIGCHLD, SIG_IGN);
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    st.dpy = XOpenDisplay(display);
    if (!st.dpy) {
        fprintf(stderr, "xwm: cannot open display %s\n",
                display ? display : getenv("DISPLAY"));
        return 1;
    }

    st.screen = DefaultScreen(st.dpy);
    st.root = RootWindow(st.dpy, st.screen);
    st.sw = DisplayWidth(st.dpy, st.screen);
    st.sh = DisplayHeight(st.dpy, st.screen);
    st.cmap = DefaultColormap(st.dpy, st.screen);

    xwm_atoms_init(&st);

    st.font = XLoadQueryFont(st.dpy, st.cfg.font_name);
    if (!st.font)
        st.font = XLoadQueryFont(st.dpy, "fixed");
    if (!st.font) {
        fprintf(stderr, "xwm: cannot load font\n");
        return 1;
    }

    st.gc = XCreateGC(st.dpy, st.root, 0, NULL);
    XSetFont(st.dpy, st.gc, st.font->fid);
    st.cur_normal = XCreateFontCursor(st.dpy, XC_left_ptr);
    st.cur_move = XCreateFontCursor(st.dpy, XC_fleur);
    st.cur_resize = XCreateFontCursor(st.dpy, XC_bottom_right_corner);
    XDefineCursor(st.dpy, st.root, st.cur_normal);

    XSetErrorHandler(xerror_wm_taken);
    wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask |
                    ColormapChangeMask | ButtonPressMask | ButtonReleaseMask |
                    KeyPressMask | EnterWindowMask | LeaveWindowMask |
                    PropertyChangeMask;
    XChangeWindowAttributes(st.dpy, st.root, CWEventMask, &wa);
    XSync(st.dpy, False);
    XSetErrorHandler(xerror_ignore);

    xwm_menu_init(&st);
    xwm_scan_clients(&st);

    st.running = true;
    fprintf(stderr, "xwm: running on %s (from-scratch engine)\n",
            DisplayString(st.dpy));

    while (st.running) {
        XNextEvent(st.dpy, &ev);
        xwm_dispatch(&st, &ev);
    }

    /* teardown */
    for (i = 0; i < XWM_MAX_CLIENTS; i++) {
        if (st.clients[i].exists)
            xwm_unmanage(&st, &st.clients[i]);
    }
    if (st.menu_win)
        XDestroyWindow(st.dpy, st.menu_win);
    if (st.font)
        XFreeFont(st.dpy, st.font);
    XFreeGC(st.dpy, st.gc);
    XCloseDisplay(st.dpy);
    free(st.config_path);
    (void)restart_argv;
    return 0;
}
