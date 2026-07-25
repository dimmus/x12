/* SPDX-License-Identifier: MIT */
#include "xwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MENU_PAD 6
#define MENU_ITEM_H 20

static void
menu_paint(XwmState *s)
{
    int i;

    XSetWindowBackground(s->dpy, s->menu_win, s->cfg.col_menu_bg);
    XClearWindow(s->dpy, s->menu_win);
    if (s->font)
        XSetFont(s->dpy, s->gc, s->font->fid);

    for (i = 0; i < s->nmenu; i++) {
        int y = MENU_PAD + i * MENU_ITEM_H;
        unsigned long bg =
            (i == s->menu_sel) ? s->cfg.col_menu_hi : s->cfg.col_menu_bg;
        unsigned long fg = s->cfg.col_menu_fg;

        XSetForeground(s->dpy, s->gc, bg);
        XFillRectangle(s->dpy, s->menu_win, s->gc, 0, y, (unsigned)s->menu_w,
                       MENU_ITEM_H);
        XSetForeground(s->dpy, s->gc, fg);
        XDrawString(s->dpy, s->menu_win, s->gc, MENU_PAD,
                    y + MENU_ITEM_H - 6, s->menu[i].label,
                    (int)strlen(s->menu[i].label));
    }
}

static void
menu_add(XwmState *s, const char *label, int action)
{
    if (s->nmenu >= XWM_MAX_MENU)
        return;
    snprintf(s->menu[s->nmenu].label, sizeof(s->menu[s->nmenu].label), "%s",
             label);
    s->menu[s->nmenu].action = action;
    s->nmenu++;
}

void
xwm_menu_init(XwmState *s)
{
    XSetWindowAttributes attr;
    int i, tw, w = 140;

    s->nmenu = 0;
    menu_add(s, "New Terminal", XWM_ACT_TERM);
    menu_add(s, "Iconify", XWM_ACT_ICONIFY);
    menu_add(s, "Raise", XWM_ACT_RAISE);
    menu_add(s, "Lower", XWM_ACT_LOWER);
    menu_add(s, "Close", XWM_ACT_CLOSE);
    menu_add(s, "Kill", XWM_ACT_KILL);
    menu_add(s, "Restart", XWM_ACT_RESTART);
    menu_add(s, "Exit", XWM_ACT_QUIT);

    if (s->font) {
        for (i = 0; i < s->nmenu; i++) {
            tw = XTextWidth(s->font, s->menu[i].label,
                            (int)strlen(s->menu[i].label));
            if (tw + 2 * MENU_PAD > w)
                w = tw + 2 * MENU_PAD;
        }
    }

    s->menu_w = w;
    s->menu_h = MENU_PAD * 2 + s->nmenu * MENU_ITEM_H;
    s->menu_sel = -1;
    s->menu_open = false;

    attr.background_pixel = s->cfg.col_menu_bg;
    attr.border_pixel = s->cfg.col_frame_focus;
    attr.override_redirect = True;
    attr.save_under = True;
    attr.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
                      PointerMotionMask | LeaveWindowMask;
    s->menu_win = XCreateWindow(
        s->dpy, s->root, 0, 0, (unsigned)s->menu_w, (unsigned)s->menu_h, 1,
        CopyFromParent, InputOutput, CopyFromParent,
        CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWSaveUnder |
            CWEventMask,
        &attr);
}

void
xwm_menu_show(XwmState *s, int x, int y)
{
    if (x + s->menu_w > s->sw)
        x = s->sw - s->menu_w;
    if (y + s->menu_h > s->sh)
        y = s->sh - s->menu_h;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    s->menu_x = x;
    s->menu_y = y;
    s->menu_sel = -1;
    XMoveResizeWindow(s->dpy, s->menu_win, x, y, (unsigned)s->menu_w,
                      (unsigned)s->menu_h);
    XMapRaised(s->dpy, s->menu_win);
    s->menu_open = true;
    menu_paint(s);
    XGrabPointer(s->dpy, s->menu_win, True,
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     LeaveWindowMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
}

void
xwm_menu_hide(XwmState *s)
{
    if (!s->menu_open)
        return;
    XUngrabPointer(s->dpy, CurrentTime);
    XUnmapWindow(s->dpy, s->menu_win);
    s->menu_open = false;
    s->menu_sel = -1;
}

void
xwm_menu_redraw(XwmState *s)
{
    if (s->menu_open)
        menu_paint(s);
}

void
xwm_menu_motion(XwmState *s, int y)
{
    int sel = (y - MENU_PAD) / MENU_ITEM_H;
    if (sel < 0 || sel >= s->nmenu)
        sel = -1;
    if (sel != s->menu_sel) {
        s->menu_sel = sel;
        menu_paint(s);
    }
}

void
xwm_menu_select(XwmState *s)
{
    int act;
    XwmClient *c = s->focus;

    if (!s->menu_open || s->menu_sel < 0 || s->menu_sel >= s->nmenu) {
        xwm_menu_hide(s);
        return;
    }
    act = s->menu[s->menu_sel].action;
    xwm_menu_hide(s);

    switch (act) {
    case XWM_ACT_TERM:
        if (fork() == 0) {
            execl("/bin/sh", "sh", "-c", s->cfg.term_cmd, (char *)NULL);
            _exit(127);
        }
        break;
    case XWM_ACT_ICONIFY:
        if (c)
            xwm_iconify(s, c);
        break;
    case XWM_ACT_RAISE:
        if (c)
            xwm_raise(s, c);
        break;
    case XWM_ACT_LOWER:
        if (c)
            xwm_lower(s, c);
        break;
    case XWM_ACT_CLOSE:
        if (c)
            xwm_close_client(s, c);
        break;
    case XWM_ACT_KILL:
        if (c)
            xwm_kill_client(s, c);
        break;
    case XWM_ACT_RESTART:
        s->running = false;
        /* caller may exec self; for now just quit cleanly */
        break;
    case XWM_ACT_QUIT:
        s->running = false;
        break;
    default:
        break;
    }
}
