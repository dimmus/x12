/* SPDX-License-Identifier: MIT */
#include "xwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long
get_wm_state(XwmState *s, Window w)
{
    Atom type = None;
    int fmt = 0;
    unsigned long n = 0, bytes = 0;
    unsigned char *data = NULL;
    long state = WithdrawnState;

    if (XGetWindowProperty(s->dpy, w, xwm_atom(s, ATOM_WM_STATE), 0, 2, False,
                           xwm_atom(s, ATOM_WM_STATE), &type, &fmt, &n, &bytes,
                           &data) == Success &&
        data && n >= 1)
        state = ((long *)data)[0];
    if (data)
        XFree(data);
    return state;
}

void
xwm_set_state(XwmState *s, XwmClient *c, long state)
{
    long data[2] = { state, None };
    XChangeProperty(s->dpy, c->client, xwm_atom(s, ATOM_WM_STATE),
                    xwm_atom(s, ATOM_WM_STATE), 32, PropModeReplace,
                    (unsigned char *)data, 2);
}

static char *
read_name(XwmState *s, Window w)
{
    char *name = NULL;
    XTextProperty tp;
    Atom type = None;
    int fmt = 0;
    unsigned long n = 0, bytes = 0;
    unsigned char *data = NULL;

    if (XGetWindowProperty(s->dpy, w, xwm_atom(s, ATOM_NET_WM_NAME), 0, 256,
                           False, xwm_atom(s, ATOM_UTF8_STRING), &type, &fmt,
                           &n, &bytes, &data) == Success &&
        data && n > 0) {
        name = strndup((char *)data, (size_t)n);
        XFree(data);
        return name;
    }
    if (data)
        XFree(data);
    if (XFetchName(s->dpy, w, &name) && name)
        return name;
    if (XGetWMName(s->dpy, w, &tp) && tp.value) {
        name = strndup((char *)tp.value, tp.nitems);
        XFree(tp.value);
        return name;
    }
    return strdup("Untitled");
}

XwmClient *
xwm_client_of(XwmState *s, Window w)
{
    int i;
    for (i = 0; i < XWM_MAX_CLIENTS; i++) {
        XwmClient *c = &s->clients[i];
        int b;
        if (!c->exists)
            continue;
        if (c->frame == w || c->title == w || c->client == w || c->icon == w)
            return c;
        for (b = 0; b < XWM_BTN_COUNT; b++)
            if (c->buttons[b] == w)
                return c;
    }
    return NULL;
}

static XwmClient *
alloc_client(XwmState *s)
{
    int i;
    for (i = 0; i < XWM_MAX_CLIENTS; i++) {
        if (!s->clients[i].exists) {
            memset(&s->clients[i], 0, sizeof(s->clients[i]));
            s->clients[i].exists = true;
            s->nclients++;
            return &s->clients[i];
        }
    }
    return NULL;
}

void
xwm_configure_client(XwmState *s, XwmClient *c)
{
    XConfigureEvent ce;

    XMoveResizeWindow(s->dpy, c->client, XWM_BORDER, XWM_TITLE_H + XWM_BORDER,
                      (unsigned)c->cw, (unsigned)c->ch);
    ce.type = ConfigureNotify;
    ce.display = s->dpy;
    ce.event = c->client;
    ce.window = c->client;
    ce.x = c->x + XWM_BORDER;
    ce.y = c->y + XWM_TITLE_H + XWM_BORDER;
    ce.width = c->cw;
    ce.height = c->ch;
    ce.border_width = 0;
    ce.above = None;
    ce.override_redirect = False;
    XSendEvent(s->dpy, c->client, False, StructureNotifyMask, (XEvent *)&ce);
}

void
xwm_redraw_frame(XwmState *s, XwmClient *c)
{
    unsigned long bg =
        (c == s->focus) ? s->cfg.col_frame_focus : s->cfg.col_frame;
    unsigned long fg =
        (c == s->focus) ? s->cfg.col_title_fg : s->cfg.col_title_fg_unfocus;
    int tw = c->w - XWM_BORDER * 2 - XWM_BTN_W * XWM_BTN_COUNT;
    const char *name = c->name ? c->name : "";

    XSetWindowBackground(s->dpy, c->frame, bg);
    XClearWindow(s->dpy, c->frame);
    XSetWindowBackground(s->dpy, c->title, bg);
    XClearWindow(s->dpy, c->title);
    XSetForeground(s->dpy, s->gc, fg);
    if (s->font)
        XSetFont(s->dpy, s->gc, s->font->fid);
    XDrawString(s->dpy, c->title, s->gc, 6,
                (XWM_TITLE_H + (s->font ? s->font->ascent : 12)) / 2, name,
                (int)strlen(name));

    /* iconify button "_" */
    XSetWindowBackground(s->dpy, c->buttons[XWM_BTN_ICONIFY], s->cfg.col_button);
    XClearWindow(s->dpy, c->buttons[XWM_BTN_ICONIFY]);
    XDrawString(s->dpy, c->buttons[XWM_BTN_ICONIFY], s->gc, 5,
                (XWM_TITLE_H + 10) / 2, "_", 1);
    /* close button "x" */
    XSetWindowBackground(s->dpy, c->buttons[XWM_BTN_CLOSE], s->cfg.col_button);
    XClearWindow(s->dpy, c->buttons[XWM_BTN_CLOSE]);
    XDrawString(s->dpy, c->buttons[XWM_BTN_CLOSE], s->gc, 5,
                (XWM_TITLE_H + 10) / 2, "x", 1);

    if (tw > 8)
        XDrawRectangle(s->dpy, c->frame, s->gc, 0, 0, (unsigned)(c->w - 1),
                       (unsigned)(c->h - 1));
    (void)tw;
}

static void
place_client(XwmState *s, XwmClient *c, XWindowAttributes *wa)
{
    c->cw = wa->width > 0 ? wa->width : 200;
    c->ch = wa->height > 0 ? wa->height : 120;
    c->w = c->cw + XWM_BORDER * 2;
    c->h = c->ch + XWM_TITLE_H + XWM_BORDER * 2;

    if (s->cfg.random_placement || (wa->x == 0 && wa->y == 0)) {
        c->x = 20 + (int)((unsigned)rand() % (unsigned)(s->sw / 3 + 1));
        c->y = 20 + (int)((unsigned)rand() % (unsigned)(s->sh / 3 + 1));
    } else {
        c->x = wa->x;
        c->y = wa->y;
    }
    if (c->x + c->w > s->sw)
        c->x = s->sw - c->w;
    if (c->y + c->h > s->sh)
        c->y = s->sh - c->h;
    if (c->x < 0)
        c->x = 0;
    if (c->y < 0)
        c->y = 0;
}

XwmClient *
xwm_manage(XwmState *s, Window w, XWindowAttributes *wa)
{
    XwmClient *c;
    XSetWindowAttributes attr;
    int bx;

    if (wa->override_redirect)
        return NULL;
    if (xwm_client_of(s, w))
        return xwm_client_of(s, w);

    c = alloc_client(s);
    if (!c)
        return NULL;

    c->client = w;
    c->name = read_name(s, w);
    c->cmap = wa->colormap;
    place_client(s, c, wa);

    attr.background_pixel = s->cfg.col_frame;
    attr.border_pixel = s->cfg.col_frame;
    attr.event_mask = SubstructureRedirectMask | SubstructureNotifyMask |
                      ButtonPressMask | ButtonReleaseMask | ExposureMask |
                      EnterWindowMask;
    attr.override_redirect = True;
    c->frame = XCreateWindow(s->dpy, s->root, c->x, c->y, (unsigned)c->w,
                             (unsigned)c->h, 0, CopyFromParent, InputOutput,
                             CopyFromParent,
                             CWBackPixel | CWBorderPixel | CWEventMask |
                                 CWOverrideRedirect,
                             &attr);

    attr.event_mask = ButtonPressMask | ButtonReleaseMask | ExposureMask |
                      EnterWindowMask | Button1MotionMask;
    c->title = XCreateWindow(s->dpy, c->frame, XWM_BORDER, XWM_BORDER,
                             (unsigned)(c->w - XWM_BORDER * 2 -
                                        XWM_BTN_W * XWM_BTN_COUNT),
                             XWM_TITLE_H, 0, CopyFromParent, InputOutput,
                             CopyFromParent, CWBackPixel | CWEventMask, &attr);

    for (bx = 0; bx < XWM_BTN_COUNT; bx++) {
        attr.event_mask = ButtonPressMask | ButtonReleaseMask | ExposureMask;
        c->buttons[bx] = XCreateWindow(
            s->dpy, c->frame,
            c->w - XWM_BORDER - XWM_BTN_W * (XWM_BTN_COUNT - bx), XWM_BORDER,
            XWM_BTN_W, XWM_TITLE_H, 0, CopyFromParent, InputOutput,
            CopyFromParent, CWBackPixel | CWEventMask, &attr);
    }

    attr.event_mask = ButtonPressMask | ButtonReleaseMask | ExposureMask |
                      Button1MotionMask;
    attr.background_pixel = s->cfg.col_frame;
    c->icon = XCreateWindow(s->dpy, s->root, c->x, c->y, XWM_ICON_W, XWM_ICON_H,
                            1, CopyFromParent, InputOutput, CopyFromParent,
                            CWBackPixel | CWEventMask | CWOverrideRedirect,
                            &attr);
    c->ix = c->x;
    c->iy = c->y;

    XAddToSaveSet(s->dpy, w);
    XSetWindowBorderWidth(s->dpy, w, 0);
    /* Reparent of a mapped window generates UnmapNotify — ignore it. */
    if (wa->map_state == IsViewable)
        c->ignore_unmap++;
    XReparentWindow(s->dpy, w, c->frame, XWM_BORDER,
                    XWM_TITLE_H + XWM_BORDER);
    XSelectInput(s->dpy, w, PropertyChangeMask | StructureNotifyMask |
                                EnterWindowMask | ColormapChangeMask);

    XMapWindow(s->dpy, c->title);
    for (bx = 0; bx < XWM_BTN_COUNT; bx++)
        XMapWindow(s->dpy, c->buttons[bx]);
    XMapWindow(s->dpy, w);
    XMapWindow(s->dpy, c->frame);
    xwm_configure_client(s, c);
    xwm_set_state(s, c, NormalState);
    xwm_redraw_frame(s, c);
    xwm_set_focus(s, c);
    XFlush(s->dpy);
    return c;
}

void
xwm_unmanage(XwmState *s, XwmClient *c)
{
    if (!c || !c->exists)
        return;
    if (s->focus == c)
        s->focus = NULL;
    XUnmapWindow(s->dpy, c->frame);
    XUnmapWindow(s->dpy, c->icon);
    XReparentWindow(s->dpy, c->client, s->root, c->x, c->y);
    XRemoveFromSaveSet(s->dpy, c->client);
    XDestroyWindow(s->dpy, c->frame);
    XDestroyWindow(s->dpy, c->icon);
    free(c->name);
    memset(c, 0, sizeof(*c));
    s->nclients--;
}

void
xwm_set_focus(XwmState *s, XwmClient *c)
{
    XwmClient *prev = s->focus;
    if (c && c->iconic)
        return;
    s->focus = c;
    if (prev && prev->exists)
        xwm_redraw_frame(s, prev);
    if (c && c->exists) {
        XSetInputFocus(s->dpy, c->client, RevertToPointerRoot, CurrentTime);
        XInstallColormap(s->dpy, c->cmap ? c->cmap : s->cmap);
        xwm_raise(s, c);
        xwm_redraw_frame(s, c);
    }
}

void
xwm_raise(XwmState *s, XwmClient *c)
{
    if (c && c->exists && !c->iconic)
        XRaiseWindow(s->dpy, c->frame);
}

void
xwm_lower(XwmState *s, XwmClient *c)
{
    if (c && c->exists && !c->iconic)
        XLowerWindow(s->dpy, c->frame);
}

void
xwm_iconify(XwmState *s, XwmClient *c)
{
    if (!c || c->iconic)
        return;
    c->iconic = true;
    XUnmapWindow(s->dpy, c->frame);
    XMapWindow(s->dpy, c->icon);
    XClearWindow(s->dpy, c->icon);
    XSetForeground(s->dpy, s->gc, s->cfg.col_title_fg);
    if (c->name)
        XDrawString(s->dpy, c->icon, s->gc, 4, 14, c->name,
                    (int)strlen(c->name));
    xwm_set_state(s, c, IconicState);
    if (s->focus == c)
        s->focus = NULL;
}

void
xwm_deiconify(XwmState *s, XwmClient *c)
{
    if (!c || !c->iconic)
        return;
    c->iconic = false;
    XUnmapWindow(s->dpy, c->icon);
    XMapWindow(s->dpy, c->frame);
    xwm_set_state(s, c, NormalState);
    xwm_set_focus(s, c);
}

void
xwm_close_client(XwmState *s, XwmClient *c)
{
    Atom *protocols = NULL;
    int n = 0, i;
    bool supports_delete = false;

    if (!c)
        return;
    if (XGetWMProtocols(s->dpy, c->client, &protocols, &n)) {
        for (i = 0; i < n; i++) {
            if (protocols[i] == xwm_atom(s, ATOM_WM_DELETE_WINDOW))
                supports_delete = true;
        }
        XFree(protocols);
    }
    if (supports_delete) {
        XEvent ev;
        memset(&ev, 0, sizeof(ev));
        ev.xclient.type = ClientMessage;
        ev.xclient.window = c->client;
        ev.xclient.message_type = xwm_atom(s, ATOM_WM_PROTOCOLS);
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = (long)xwm_atom(s, ATOM_WM_DELETE_WINDOW);
        ev.xclient.data.l[1] = CurrentTime;
        XSendEvent(s->dpy, c->client, False, NoEventMask, &ev);
    } else {
        xwm_kill_client(s, c);
    }
}

void
xwm_kill_client(XwmState *s, XwmClient *c)
{
    if (c)
        XKillClient(s->dpy, c->client);
}

void
xwm_scan_clients(XwmState *s)
{
    Window root_r, parent, *children = NULL;
    unsigned n = 0, i;

    if (!XQueryTree(s->dpy, s->root, &root_r, &parent, &children, &n) ||
        !children)
        return;
    for (i = 0; i < n; i++) {
        XWindowAttributes wa;
        if (!XGetWindowAttributes(s->dpy, children[i], &wa))
            continue;
        if (wa.override_redirect || wa.map_state != IsViewable)
            continue;
        if (get_wm_state(s, children[i]) == IconicState)
            continue;
        xwm_manage(s, children[i], &wa);
    }
    XFree(children);
}

static void
drag_loop(XwmState *s, XwmClient *c, int x_root, int y_root, bool resize)
{
    int ox = c->x, oy = c->y, ow = c->w, oh = c->h;
    int dx = x_root - c->x, dy = y_root - c->y;
    XEvent ev;

    XGrabPointer(s->dpy, s->root, True,
                 PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
                 GrabModeAsync, None, resize ? s->cur_resize : s->cur_move,
                 CurrentTime);
    for (;;) {
        XMaskEvent(s->dpy, PointerMotionMask | ButtonReleaseMask, &ev);
        if (ev.type == MotionNotify) {
            if (resize) {
                int nw = ow + (ev.xmotion.x_root - x_root);
                int nh = oh + (ev.xmotion.y_root - y_root);
                if (nw < 80)
                    nw = 80;
                if (nh < 60)
                    nh = 60;
                c->w = nw;
                c->h = nh;
                c->cw = c->w - XWM_BORDER * 2;
                c->ch = c->h - XWM_TITLE_H - XWM_BORDER * 2;
                XMoveResizeWindow(s->dpy, c->frame, c->x, c->y, (unsigned)c->w,
                                  (unsigned)c->h);
                XResizeWindow(s->dpy, c->title,
                              (unsigned)(c->w - XWM_BORDER * 2 -
                                         XWM_BTN_W * XWM_BTN_COUNT),
                              XWM_TITLE_H);
                {
                    int b;
                    for (b = 0; b < XWM_BTN_COUNT; b++)
                        XMoveWindow(s->dpy, c->buttons[b],
                                    c->w - XWM_BORDER -
                                        XWM_BTN_W * (XWM_BTN_COUNT - b),
                                    XWM_BORDER);
                }
                xwm_configure_client(s, c);
            } else {
                c->x = ev.xmotion.x_root - dx;
                c->y = ev.xmotion.y_root - dy;
                XMoveWindow(s->dpy, c->frame, c->x, c->y);
            }
        } else if (ev.type == ButtonRelease) {
            break;
        }
    }
    XUngrabPointer(s->dpy, CurrentTime);
    (void)ox;
    (void)oy;
}

void
xwm_move_interactive(XwmState *s, XwmClient *c, int x_root, int y_root)
{
    if (c && !c->iconic)
        drag_loop(s, c, x_root, y_root, false);
}

void
xwm_resize_interactive(XwmState *s, XwmClient *c, int x_root, int y_root)
{
    if (c && !c->iconic)
        drag_loop(s, c, x_root, y_root, true);
}
