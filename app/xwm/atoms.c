/* SPDX-License-Identifier: MIT */
#include "xwm.h"

#include <string.h>

void
xwm_atoms_init(XwmState *s)
{
    static const char *names[ATOM_COUNT] = {
        "WM_PROTOCOLS",
        "WM_DELETE_WINDOW",
        "WM_STATE",
        "WM_TAKE_FOCUS",
        "WM_CHANGE_STATE",
        "WM_NORMAL_HINTS",
        "_NET_WM_NAME",
        "UTF8_STRING",
    };
    int i;

    for (i = 0; i < ATOM_COUNT; i++)
        s->atoms[i] = XInternAtom(s->dpy, names[i], False);
}

Atom
xwm_atom(XwmState *s, int idx)
{
    if (idx < 0 || idx >= ATOM_COUNT)
        return None;
    return s->atoms[idx];
}
