/*
 * G1 security smoke: sandbox client cannot QueryKeymap (keylog vector).
 *
 * Usage (harness starts Xvfb with -sandbox-clients 2):
 *   deny_keylog          — connect as client 1 (full), then spawn nothing;
 *                          this binary alone connects twice? No — run twice.
 *
 * Preferred: one process opens two Display connections:
 *   dpy_full  = first connection  → client index 1 → full
 *   dpy_sand  = second connection → client index 2 → sandbox (-sandbox-clients 2)
 * Then QueryKeymap on both; sandbox map must be scrubbed after XTest key from full.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int
keymap_nonzero(const char map[32])
{
    int i;

    for (i = 0; i < 32; i++) {
        if (map[i])
            return 1;
    }
    return 0;
}

int
main(void)
{
    Display *full, *sand;
    char map_full[32], map_sand[32];
    KeyCode kc;
    int major = 0, minor = 0, evt = 0, err = 0;

    full = XOpenDisplay(NULL);
    if (!full) {
        fprintf(stderr, "deny_keylog: cannot open display (full)\n");
        return 1;
    }
    sand = XOpenDisplay(NULL);
    if (!sand) {
        fprintf(stderr, "deny_keylog: cannot open display (sandbox)\n");
        return 1;
    }

    if (!XTestQueryExtension(full, &evt, &err, &major, &minor)) {
        fprintf(stderr, "deny_keylog: XTEST not available\n");
        return 1;
    }

    kc = XKeysymToKeycode(full, XK_a);
    if (!kc)
        kc = 38; /* common 'a' on pc105 when XKB symbol lookup is incomplete */
    printf("deny_keylog: using keycode %d\n", (int)kc);

    /* Hold a key down via full client. */
    XTestFakeKeyEvent(full, kc, True, CurrentTime);
    XFlush(full);
    usleep(50000);

    memset(map_full, 0, sizeof(map_full));
    memset(map_sand, 0, sizeof(map_sand));
    XQueryKeymap(full, map_full);
    XQueryKeymap(sand, map_sand);

    XTestFakeKeyEvent(full, kc, False, CurrentTime);
    XFlush(full);

    if (!keymap_nonzero(map_full)) {
        fprintf(stderr, "deny_keylog: FAIL — full client QueryKeymap empty (expected key down)\n");
        return 1;
    }
    if (keymap_nonzero(map_sand)) {
        fprintf(stderr, "deny_keylog: FAIL — sandbox client saw keymap bits (keylog)\n");
        return 1;
    }

    printf("deny_keylog: PASS — sandbox QueryKeymap scrubbed; full saw key state\n");
    XCloseDisplay(sand);
    XCloseDisplay(full);
    return 0;
}
