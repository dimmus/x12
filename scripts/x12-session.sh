#!/usr/bin/env bash
# Interactive X12 session: Xephyr/Xvfb + xwm + xterm.
# Usage: ./scripts/x12-session.sh [xvfb|xephyr]
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
PREFIX="${X12_PREFIX:-$ROOT/prefix}"
MODE="${1:-xvfb}"
DISP_NUM="${X12_DISPLAY_NUM:-98}"

export PATH="$PREFIX/bin:$BUILD/app/xwm:$BUILD/app/xterm:$BUILD/app/xclock:$PATH"
export LD_LIBRARY_PATH="$BUILD/lib/src/x11:$BUILD/lib/xcb:$BUILD/lib/src/xau:$BUILD/lib/src/xdmcp:${LD_LIBRARY_PATH:-}"
export X12_BUILD_DIR="$BUILD"

XVFB="$BUILD/server/hw/vfb/Xvfb"
XEPHYR="$BUILD/server/hw/kdrive/ephyr/Xephyr"
XWM="$BUILD/app/xwm/xwm"
XTERM="$BUILD/app/xterm/xterm"

need() { [[ -x "$1" ]] || { echo "missing $1 — build with -Dbuild-app=true" >&2; exit 1; }; }
need "$XWM"
need "$XTERM"

cleanup() {
  kill "$WM_PID" 2>/dev/null || true
  kill "$SRV_PID" 2>/dev/null || true
}
trap cleanup EXIT

case "$MODE" in
  xvfb)
    need "$XVFB"
    "$XVFB" ":$DISP_NUM" -ac -screen 0 1280x800x24 \
      -extension XFree86-Bigfont \
      -fp /usr/share/fonts/X11/misc,/usr/share/fonts/X11/Type1 \
      -client-level full &
    SRV_PID=$!
    ;;
  xephyr)
    need "$XEPHYR"
    # Nested: require an existing DISPLAY for the host window.
    "$XEPHYR" ":$DISP_NUM" -ac -screen 1280x800 \
      -extension XFree86-Bigfont \
      -client-level full &
    SRV_PID=$!
    ;;
  *)
    echo "usage: $0 [xvfb|xephyr]" >&2
    exit 2
    ;;
esac

for _ in $(seq 1 80); do
  [[ -S "/tmp/.X11-unix/X${DISP_NUM}" ]] && break
  sleep 0.1
done
export DISPLAY=":$DISP_NUM"
unset XAUTHORITY

"$XWM" -f "$ROOT/app/xwm/system.xwmrc" &
WM_PID=$!
sleep 0.3
"$XTERM" -geometry 80x24+40+40 &
"$XTERM" -geometry 80x24+160+160 &
if [[ -x "$BUILD/app/xclock/xclock" ]]; then
  "$BUILD/app/xclock/xclock" -geometry 100x100-0+0 &
fi

echo "x12-session: DISPLAY=$DISPLAY xwm pid=$WM_PID ($MODE)"
echo "x12-session: root menu = button1 on desktop"
wait "$WM_PID"
