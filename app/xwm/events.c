/* SPDX-License-Identifier: MIT */
#include "xwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
on_map_request(XwmState *s, XMapRequestEvent *ev)
{
    XWindowAttributes wa;

    if (!XGetWindowAttributes(s->dpy, ev->window, &wa))
        return;
    if (wa.override_redirect) {
        XMapWindow(s->dpy, ev->window);
        return;
    }
    if (!xwm_client_of(s, ev->window))
        xwm_manage(s, ev->window, &wa);
    else
        XMapWindow(s->dpy, ev->window);
}

static void
on_configure_request(XwmState *s, XConfigureRequestEvent *ev)
{
    XwmClient *c = xwm_client_of(s, ev->window);
    XWindowChanges wc;

    wc.x = ev->x;
    wc.y = ev->y;
    wc.width = ev->width;
    wc.height = ev->height;
    wc.border_width = ev->border_width;
    wc.sibling = ev->above;
    wc.stack_mode = ev->detail;

    if (!c) {
        XConfigureWindow(s->dpy, ev->window, (unsigned)ev->value_mask, &wc);
        return;
    }

    if (ev->value_mask & CWX)
        c->x = ev->x;
    if (ev->value_mask & CWY)
        c->y = ev->y;
    if (ev->value_mask & CWWidth) {
        c->cw = ev->width;
        c->w = c->cw + XWM_BORDER * 2;
    }
    if (ev->value_mask & CWHeight) {
        c->ch = ev->height;
        c->h = c->ch + XWM_TITLE_H + XWM_BORDER * 2;
    }

    XMoveResizeWindow(s->dpy, c->frame, c->x, c->y, (unsigned)c->w,
                      (unsigned)c->h);
    XResizeWindow(s->dpy, c->title,
                  (unsigned)(c->w - XWM_BORDER * 2 - XWM_BTN_W * XWM_BTN_COUNT),
                  XWM_TITLE_H);
    {
        int b;
        for (b = 0; b < XWM_BTN_COUNT; b++)
            XMoveWindow(s->dpy, c->buttons[b],
                        c->w - XWM_BORDER - XWM_BTN_W * (XWM_BTN_COUNT - b),
                        XWM_BORDER);
    }
    xwm_configure_client(s, c);
    xwm_redraw_frame(s, c);
}

static void
on_unmap(XwmState *s, XUnmapEvent *ev)
{
    XwmClient *c = xwm_client_of(s, ev->window);
    if (!c || c->client != ev->window)
        return;
    if (c->ignore_unmap > 0) {
        c->ignore_unmap--;
        return;
    }
    if (c->iconic)
        return;
    xwm_unmanage(s, c);
}

static void
on_destroy(XwmState *s, XDestroyWindowEvent *ev)
{
    XwmClient *c = xwm_client_of(s, ev->window);
    if (c)
        xwm_unmanage(s, c);
}

static void
on_property(XwmState *s, XPropertyEvent *ev)
{
    XwmClient *c = xwm_client_of(s, ev->window);
    char *name = NULL;
    XTextProperty tp;
    Atom type = None;
    int fmt = 0;
    unsigned long n = 0, bytes = 0;
    unsigned char *data = NULL;

    if (!c || c->client != ev->window)
        return;

    if (ev->atom == XA_WM_NAME || ev->atom == xwm_atom(s, ATOM_NET_WM_NAME)) {
        if (XGetWindowProperty(s->dpy, c->client, xwm_atom(s, ATOM_NET_WM_NAME),
                               0, 256, False, xwm_atom(s, ATOM_UTF8_STRING),
                               &type, &fmt, &n, &bytes, &data) == Success &&
            data && n > 0) {
            name = strndup((char *)data, (size_t)n);
            XFree(data);
        } else {
            if (data)
                XFree(data);
            if (XFetchName(s->dpy, c->client, &name) && name) {
                /* ok */
            } else if (XGetWMName(s->dpy, c->client, &tp) && tp.value) {
                name = strndup((char *)tp.value, tp.nitems);
                XFree(tp.value);
            }
        }
        if (name) {
            free(c->name);
            c->name = name;
            xwm_redraw_frame(s, c);
        }
    }
}

static void
on_button_press(XwmState *s, XButtonEvent *ev)
{
    XwmClient *c;

    if (s->menu_open) {
        if (ev->window == s->menu_win) {
            xwm_menu_motion(s, ev->y);
            return;
        }
        xwm_menu_hide(s);
        return;
    }

    if (ev->window == s->root) {
        if (ev->button == Button1 || ev->button == Button3) {
            xwm_menu_show(s, ev->x_root, ev->y_root);
            return;
        }
        return;
    }

    c = xwm_client_of(s, ev->window);
    if (!c)
        return;

    if (c->iconic && ev->window == c->icon) {
        if (ev->button == Button1)
            xwm_deiconify(s, c);
        return;
    }

    xwm_set_focus(s, c);

    if (ev->window == c->buttons[XWM_BTN_CLOSE]) {
        if (ev->button == Button1)
            xwm_close_client(s, c);
        return;
    }
    if (ev->window == c->buttons[XWM_BTN_ICONIFY]) {
        if (ev->button == Button1)
            xwm_iconify(s, c);
        return;
    }

    if (ev->window == c->title || ev->window == c->frame) {
        if (ev->button == Button1)
            xwm_move_interactive(s, c, ev->x_root, ev->y_root);
        else if (ev->button == Button2 || ev->button == Button3)
            xwm_resize_interactive(s, c, ev->x_root, ev->y_root);
    }
}

static void
on_button_release(XwmState *s, XButtonEvent *ev)
{
    if (s->menu_open && ev->window == s->menu_win) {
        xwm_menu_motion(s, ev->y);
        xwm_menu_select(s);
    }
}

static void
on_motion(XwmState *s, XMotionEvent *ev)
{
    if (s->menu_open && ev->window == s->menu_win)
        xwm_menu_motion(s, ev->y);
}

static void
on_expose(XwmState *s, XExposeEvent *ev)
{
    XwmClient *c;

    if (ev->count != 0)
        return;
    if (s->menu_open && ev->window == s->menu_win) {
        xwm_menu_redraw(s);
        return;
    }
    c = xwm_client_of(s, ev->window);
    if (!c)
        return;
    if (c->iconic && ev->window == c->icon) {
        XClearWindow(s->dpy, c->icon);
        XSetForeground(s->dpy, s->gc, s->cfg.col_title_fg);
        if (c->name)
            XDrawString(s->dpy, c->icon, s->gc, 4, 14, c->name,
                        (int)strlen(c->name));
        return;
    }
    xwm_redraw_frame(s, c);
}

static void
on_client_message(XwmState *s, XClientMessageEvent *ev)
{
    XwmClient *c = xwm_client_of(s, ev->window);
    if (!c)
        return;
    if (ev->message_type == xwm_atom(s, ATOM_WM_CHANGE_STATE) &&
        ev->format == 32) {
        if (ev->data.l[0] == IconicState)
            xwm_iconify(s, c);
        else if (ev->data.l[0] == NormalState)
            xwm_deiconify(s, c);
    }
}

static void
on_key(XwmState *s, XKeyEvent *ev)
{
    KeySym ks = XLookupKeysym(ev, 0);
    XwmClient *c = s->focus;
    int i, n;

    if (!(ev->state & Mod1Mask))
        return;

    if (ks == XK_F4 && c) {
        xwm_close_client(s, c);
    } else if (ks == XK_F9 && c) {
        xwm_iconify(s, c);
    } else if (ks == XK_Escape && s->menu_open) {
        xwm_menu_hide(s);
    } else if (ks == XK_Tab) {
        if (s->nclients <= 0)
            return;
        if (!c) {
            for (i = 0; i < XWM_MAX_CLIENTS; i++) {
                if (s->clients[i].exists && !s->clients[i].iconic) {
                    xwm_set_focus(s, &s->clients[i]);
                    return;
                }
            }
            return;
        }
        for (i = 0; i < XWM_MAX_CLIENTS; i++) {
            if (&s->clients[i] == c)
                break;
        }
        for (n = 1; n <= XWM_MAX_CLIENTS; n++) {
            XwmClient *next = &s->clients[(i + n) % XWM_MAX_CLIENTS];
            if (next->exists && !next->iconic) {
                xwm_set_focus(s, next);
                return;
            }
        }
    }
}

void
xwm_dispatch(XwmState *s, XEvent *ev)
{
    if (xwm_log_level >= XWM_LOG_TRACE) {
        Window w = 0;
        switch (ev->type) {
        case MapRequest:
            w = ev->xmaprequest.window;
            break;
        case ConfigureRequest:
            w = ev->xconfigurerequest.window;
            break;
        case UnmapNotify:
            w = ev->xunmap.window;
            break;
        case DestroyNotify:
            w = ev->xdestroywindow.window;
            break;
        case ButtonPress:
        case ButtonRelease:
            w = ev->xbutton.window;
            break;
        case PropertyNotify:
            w = ev->xproperty.window;
            break;
        case ClientMessage:
            w = ev->xclient.window;
            break;
        default:
            break;
        }
        XWM_TRC("event %s window=0x%lx", xwm_event_name(ev->type),
                (unsigned long)w);
    }

    switch (ev->type) {
    case MapRequest:
        on_map_request(s, &ev->xmaprequest);
        break;
    case ConfigureRequest:
        on_configure_request(s, &ev->xconfigurerequest);
        break;
    case UnmapNotify:
        on_unmap(s, &ev->xunmap);
        break;
    case DestroyNotify:
        on_destroy(s, &ev->xdestroywindow);
        break;
    case PropertyNotify:
        on_property(s, &ev->xproperty);
        break;
    case ButtonPress:
        on_button_press(s, &ev->xbutton);
        break;
    case ButtonRelease:
        on_button_release(s, &ev->xbutton);
        break;
    case MotionNotify:
        on_motion(s, &ev->xmotion);
        break;
    case Expose:
        on_expose(s, &ev->xexpose);
        break;
    case LeaveNotify:
        if (s->menu_open && ev->xcrossing.window == s->menu_win)
            xwm_menu_hide(s);
        break;
    case ClientMessage:
        on_client_message(s, &ev->xclient);
        break;
    case KeyPress:
        on_key(s, &ev->xkey);
        break;
    case MappingNotify:
        XRefreshKeyboardMapping(&ev->xmapping);
        break;
    case ColormapNotify:
        if (s->focus && ev->xcolormap.window == s->focus->client &&
            ev->xcolormap.new)
            XInstallColormap(s->dpy, ev->xcolormap.colormap);
        break;
    default:
        break;
    }
}
