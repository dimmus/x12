# xwm — X12 reference window manager

**xwm** is a from-scratch ICCCM window manager for X12 sessions: titlebars,
iconify/close, move/resize, click-to-focus, root menu, and `WM_STATE` /
`WM_DELETE_WINDOW`. It is a normal X client that owns SubstructureRedirect;
the server’s built-in compositor (X12-SURFACE) remains separate (ADR-0005,
ADR-0019).

Run at client level `user` or `full` — not `sandbox`.

```sh
# Headless smoke (also: ./tests/x12/run_wm.sh)
Xvfb :97 -screen 0 1280x800x24 &
DISPLAY=:97 xwm &
DISPLAY=:97 xterm &
```

Config: `~/.xwmrc` if present, else `$datadir/X11/xwm/system.xwmrc`
(key=value: `font`, `frame`, `frame_focus`, `title_fg`, `term`,
`random_placement`).

Session: `$datadir/X11/xwm/x12-xinitrc` or `./scripts/x12-session.sh`.

Bindings (defaults):
- Button1 on root → root menu
- Button1 on title → move; Button2/3 on title → resize
- Title buttons → iconify / close
- Alt+F4 close, Alt+F9 iconify, Alt+Tab cycle
