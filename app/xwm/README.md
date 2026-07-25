# xwm — X12 reference window manager

**xwm** is the X12 session window manager face: a twm-class ICCCM WM
(titlebars, iconify/resize/move, root menu, focus) driven by the in-tree
`twm` engine with X12 defaults (`system.xwmrc`).

The server’s built-in **compositor** (X12-SURFACE) handles buffer scanout;
**xwm** remains a client that owns SubstructureRedirect / decoration
(ADR-0005, ADR-0019). Run at client level `user` or `full` — not `sandbox`.

```sh
# Headless smoke (also: ./tests/x12/run_wm.sh)
Xvfb :97 -screen 0 1280x800x24 &
DISPLAY=:97 xwm &
DISPLAY=:97 xterm &
```

Config: `~/.xwmrc` if present, else `$datadir/X11/xwm/system.xwmrc`.
Session: `$datadir/X11/xwm/x12-xinitrc`.
