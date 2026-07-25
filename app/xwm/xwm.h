/* SPDX-License-Identifier: MIT
 * X12 reference window manager — from-scratch ICCCM engine (ADR-0019).
 */
#ifndef XWM_H
#define XWM_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define XWM_TITLE_H       22
#define XWM_BORDER        3
#define XWM_BTN_W         18
#define XWM_ICON_W        80
#define XWM_ICON_H        20
#define XWM_MAX_CLIENTS   256
#define XWM_MAX_MENU      32

enum {
    XWM_BTN_ICONIFY = 0,
    XWM_BTN_CLOSE   = 1,
    XWM_BTN_COUNT   = 2
};

typedef struct XwmConfig {
    char font_name[128];
    unsigned long col_frame;
    unsigned long col_frame_focus;
    unsigned long col_title_fg;
    unsigned long col_title_fg_unfocus;
    unsigned long col_button;
    unsigned long col_menu_bg;
    unsigned long col_menu_fg;
    unsigned long col_menu_hi;
    bool random_placement;
    bool title_focus; /* focus follows title click (click-to-focus) */
    char term_cmd[256];
} XwmConfig;

typedef struct XwmClient {
    Window frame;
    Window title;
    Window client;
    Window buttons[XWM_BTN_COUNT];
    Window icon;
    char *name;
    int x, y, w, h;       /* frame geometry */
    int cw, ch;           /* client size */
    int ix, iy;           /* icon position */
    bool iconic;
    bool shaded;
    bool exists;
    int ignore_unmap; /* swallow UnmapNotify from reparent/map we initiated */
    Colormap cmap;
} XwmClient;

typedef struct XwmMenuItem {
    char label[64];
    int action; /* see XWM_ACT_* */
} XwmMenuItem;

enum {
    XWM_ACT_NOP = 0,
    XWM_ACT_TERM,
    XWM_ACT_ICONIFY,
    XWM_ACT_RAISE,
    XWM_ACT_LOWER,
    XWM_ACT_CLOSE,
    XWM_ACT_KILL,
    XWM_ACT_RESTART,
    XWM_ACT_QUIT
};

enum {
    XWM_LOG_ERROR = 0,
    XWM_LOG_INFO  = 1,
    XWM_LOG_DEBUG = 2,
    XWM_LOG_TRACE = 3
};

typedef struct XwmState {
    Display *dpy;
    int screen;
    Window root;
    int sw, sh;
    Colormap cmap;
    GC gc;
    GC gc_inv;
    XFontStruct *font;
    Cursor cur_normal;
    Cursor cur_move;
    Cursor cur_resize;
    Atom atoms[16];
    XwmConfig cfg;
    XwmClient clients[XWM_MAX_CLIENTS];
    int nclients;
    XwmClient *focus;
    XwmMenuItem menu[XWM_MAX_MENU];
    int nmenu;
    Window menu_win;
    bool menu_open;
    int menu_sel;
    int menu_x, menu_y, menu_w, menu_h;
    bool running;
    char *config_path;
} XwmState;

extern int xwm_log_level;
void xwm_log_init(int level);
void xwm_log(int level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
const char *xwm_event_name(int type);

#define XWM_ERR(...)  xwm_log(XWM_LOG_ERROR, __VA_ARGS__)
#define XWM_INFO(...) xwm_log(XWM_LOG_INFO, __VA_ARGS__)
#define XWM_DBG(...)  xwm_log(XWM_LOG_DEBUG, __VA_ARGS__)
#define XWM_TRC(...)  xwm_log(XWM_LOG_TRACE, __VA_ARGS__)

/* atoms.c indices */
enum {
    ATOM_WM_PROTOCOLS = 0,
    ATOM_WM_DELETE_WINDOW,
    ATOM_WM_STATE,
    ATOM_WM_TAKE_FOCUS,
    ATOM_WM_CHANGE_STATE,
    ATOM_WM_NORMAL_HINTS,
    ATOM_NET_WM_NAME,
    ATOM_UTF8_STRING,
    ATOM_COUNT
};

void xwm_atoms_init(XwmState *s);
Atom xwm_atom(XwmState *s, int idx);

void xwm_config_defaults(XwmConfig *c);
int xwm_config_load(XwmState *s, const char *path);

XwmClient *xwm_client_of(XwmState *s, Window w);
XwmClient *xwm_manage(XwmState *s, Window w, XWindowAttributes *wa);
void xwm_unmanage(XwmState *s, XwmClient *c);
void xwm_set_focus(XwmState *s, XwmClient *c);
void xwm_set_state(XwmState *s, XwmClient *c, long state);
void xwm_iconify(XwmState *s, XwmClient *c);
void xwm_deiconify(XwmState *s, XwmClient *c);
void xwm_close_client(XwmState *s, XwmClient *c);
void xwm_kill_client(XwmState *s, XwmClient *c);
void xwm_raise(XwmState *s, XwmClient *c);
void xwm_lower(XwmState *s, XwmClient *c);
void xwm_redraw_frame(XwmState *s, XwmClient *c);
void xwm_configure_client(XwmState *s, XwmClient *c);
void xwm_scan_clients(XwmState *s);

void xwm_move_interactive(XwmState *s, XwmClient *c, int x_root, int y_root);
void xwm_resize_interactive(XwmState *s, XwmClient *c, int x_root, int y_root);

void xwm_menu_init(XwmState *s);
void xwm_menu_show(XwmState *s, int x, int y);
void xwm_menu_hide(XwmState *s);
void xwm_menu_motion(XwmState *s, int y);
void xwm_menu_select(XwmState *s);
void xwm_menu_redraw(XwmState *s);

void xwm_dispatch(XwmState *s, XEvent *ev);

#endif /* XWM_H */
