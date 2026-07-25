/* SPDX-License-Identifier: MIT
 *
 * xwm — X12 reference window manager (from-scratch ICCCM engine).
 *
 * Usage: xwm [-display dpy] [-f config] [-v|-vv]
 * Env:   XWM_DEBUG=0|1|2|debug|trace
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
        ev->error_code == BadMatch) {
        XWM_TRC("X error ignored: code=%d major=%d minor=%d resource=0x%lx",
                ev->error_code, ev->request_code, ev->minor_code,
                (unsigned long)ev->resourceid);
        return 0;
    }
    XWM_ERR("X error code=%d major=%d minor=%d resource=0x%lx", ev->error_code,
            ev->request_code, ev->minor_code, (unsigned long)ev->resourceid);
    return 0;
}

static int
xerror_wm_taken(Display *dpy, XErrorEvent *ev)
{
    (void)dpy;
    if (ev->error_code == BadAccess) {
        XWM_ERR("another window manager is already running on this display");
        exit(1);
    }
    return 0;
}

static void
usage(const char *argv0)
{
    fprintf(stderr,
            "Usage: %s [-display dpy] [-f config] [-v|-vv]\n"
            "  -v / -vv     debug / trace logging (or XWM_DEBUG=1|2)\n",
            argv0);
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
    int cli_level = XWM_LOG_INFO;
    int i;

    memset(&st, 0, sizeof(st));
    xwm_config_defaults(&st.cfg);

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-display") && i + 1 < argc) {
            display = argv[++i];
        } else if ((!strcmp(argv[i], "-f") || !strcmp(argv[i], "-file")) &&
                   i + 1 < argc) {
            cfg = argv[++i];
        } else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
            cli_level = XWM_LOG_DEBUG;
        } else if (!strcmp(argv[i], "-vv") || !strcmp(argv[i], "--trace")) {
            cli_level = XWM_LOG_TRACE;
        } else if (!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")) {
            cli_level = XWM_LOG_ERROR;
        } else if (!strcmp(argv[i], "-help") || !strcmp(argv[i], "--help")) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "xwm: unknown option %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    xwm_log_init(cli_level);

    if (!cfg)
        cfg = default_config(cfgbuf, sizeof(cfgbuf));
    if (cfg) {
        st.config_path = strdup(cfg);
        if (xwm_config_load(&st, cfg) != 0)
            XWM_ERR("could not load config %s (using defaults)", cfg);
        else
            XWM_INFO("config %s", cfg);
    } else {
        xwm_config_defaults(&st.cfg);
        XWM_INFO("config: built-in defaults (no system.xwmrc found)");
    }
    XWM_DBG("config font=%s term=%s random_placement=%d", st.cfg.font_name,
            st.cfg.term_cmd, st.cfg.random_placement ? 1 : 0);

    signal(SIGCHLD, SIG_IGN);
    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    XWM_INFO("opening display %s",
             display ? display : (getenv("DISPLAY") ? getenv("DISPLAY") : "(null)"));
    st.dpy = XOpenDisplay(display);
    if (!st.dpy) {
        XWM_ERR("cannot open display %s",
                display ? display : getenv("DISPLAY"));
        return 1;
    }

    st.screen = DefaultScreen(st.dpy);
    st.root = RootWindow(st.dpy, st.screen);
    st.sw = DisplayWidth(st.dpy, st.screen);
    st.sh = DisplayHeight(st.dpy, st.screen);
    st.cmap = DefaultColormap(st.dpy, st.screen);
    XWM_DBG("screen=%d root=0x%lx size=%dx%d depth=%d", st.screen,
            (unsigned long)st.root, st.sw, st.sh,
            DefaultDepth(st.dpy, st.screen));

    xwm_atoms_init(&st);
    XWM_TRC("atoms ready (WM_STATE=0x%lx WM_PROTOCOLS=0x%lx)",
            (unsigned long)xwm_atom(&st, ATOM_WM_STATE),
            (unsigned long)xwm_atom(&st, ATOM_WM_PROTOCOLS));

    st.font = XLoadQueryFont(st.dpy, st.cfg.font_name);
    if (!st.font) {
        XWM_DBG("font '%s' missing, falling back to fixed", st.cfg.font_name);
        st.font = XLoadQueryFont(st.dpy, "fixed");
    }
    if (!st.font) {
        XWM_ERR("cannot load font");
        return 1;
    }
    XWM_DBG("font loaded fid=0x%lx", (unsigned long)st.font->fid);

    st.gc = XCreateGC(st.dpy, st.root, 0, NULL);
    XSetFont(st.dpy, st.gc, st.font->fid);
    st.cur_normal = XCreateFontCursor(st.dpy, XC_left_ptr);
    st.cur_move = XCreateFontCursor(st.dpy, XC_fleur);
    st.cur_resize = XCreateFontCursor(st.dpy, XC_bottom_right_corner);
    XDefineCursor(st.dpy, st.root, st.cur_normal);

    XWM_INFO("claiming SubstructureRedirect on root");
    XSetErrorHandler(xerror_wm_taken);
    wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask |
                    ColormapChangeMask | ButtonPressMask | ButtonReleaseMask |
                    KeyPressMask | EnterWindowMask | LeaveWindowMask |
                    PropertyChangeMask;
    XChangeWindowAttributes(st.dpy, st.root, CWEventMask, &wa);
    XSync(st.dpy, False);
    XSetErrorHandler(xerror_ignore);
    XWM_INFO("WM selection OK (SubstructureRedirect acquired)");

    xwm_menu_init(&st);
    XWM_DBG("root menu ready (%d items)", st.nmenu);

    xwm_scan_clients(&st);
    XWM_INFO("startup scan: %d managed client(s)", st.nclients);

    st.running = true;
    XWM_INFO("running on %s (log_level=%d)", DisplayString(st.dpy),
             xwm_log_level);

    while (st.running) {
        XNextEvent(st.dpy, &ev);
        xwm_dispatch(&st, &ev);
    }

    XWM_INFO("shutting down (%d client(s))", st.nclients);
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
    XWM_INFO("exit");
    return 0;
}
