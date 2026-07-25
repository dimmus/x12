#!/usr/bin/env bash
# X12 reference WM smoke: Xvfb + xwm + managed client reparent/WM_STATE.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
PREFIX="${X12_PREFIX:-$ROOT/prefix}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
XWM="$BUILD/app/xwm/xwm"
CC="${CC:-cc}"
DISP_NUM="${X12_WM_DISPLAY:-89}"

need() { [[ -x "$1" ]] || { echo "missing $1" >&2; exit 1; }; }
need "$XVFB"
need "$XWM"
[[ -x "$BUILD/app/xkbcomp/xkbcomp" || -x /usr/bin/xkbcomp ]] || {
  echo "missing xkbcomp" >&2
  exit 1
}

mkdir -p "$BUILD/tests/x12"

BIN="$BUILD/tests/x12/wm_manage"
INC=(-I"$ROOT/include" -I"$BUILD")
LIBS=(
  -L"$BUILD/lib/src/x11" -lX11
  -L"$BUILD/lib/ext/xext" -lXext
  -L"$BUILD/lib/xcb" -lxcb
  -L"$BUILD/lib/src/xau" -lXau
  -L"$BUILD/lib/src/xdmcp" -lXdmcp
  -Wl,-rpath,"$BUILD/lib/src/x11:$BUILD/lib/ext/xext:$BUILD/lib/xcb:$BUILD/lib/src/xau:$BUILD/lib/src/xdmcp"
)
# shellcheck disable=SC2086
"$CC" -O2 -g ${CFLAGS:-} ${LDFLAGS:-} "$ROOT/tests/x12/wm_manage.c" -o "$BIN" \
  "${INC[@]}" "${LIBS[@]}"

LOG=/tmp/x12-wm-xvfb.log
rm -f "/tmp/.X${DISP_NUM}-lock" "/tmp/.X11-unix/X${DISP_NUM}"
"$XVFB" ":$DISP_NUM" -ac -screen 0 1280x800x24 \
  -extension XFree86-Bigfont \
  -fp /usr/share/fonts/X11/misc,/usr/share/fonts/X11/Type1 \
  -client-level full \
  >"$LOG" 2>&1 &
XVFB_PID=$!
XWM_PID=""
cleanup() {
  [[ -n "${XWM_PID}" ]] && kill "$XWM_PID" 2>/dev/null || true
  kill "$XVFB_PID" 2>/dev/null || true
  rm -f "/tmp/.X${DISP_NUM}-lock"
}
trap cleanup EXIT

for _ in $(seq 1 100); do
  [[ -S "/tmp/.X11-unix/X${DISP_NUM}" ]] && break
  if ! kill -0 "$XVFB_PID" 2>/dev/null; then
    echo "Xvfb exited early" >&2
    cat "$LOG" >&2 || true
    exit 1
  fi
  sleep 0.1
done
[[ -S "/tmp/.X11-unix/X${DISP_NUM}" ]] || {
  echo "Xvfb failed" >&2
  cat "$LOG" >&2
  exit 1
}
# Wait until keyboard init finishes (xkbcomp).
for _ in $(seq 1 100); do
  grep -q "Errors from xkbcomp are not fatal" "$LOG" 2>/dev/null && break
  sleep 0.1
done

export DISPLAY=":$DISP_NUM"
unset XAUTHORITY
export LD_LIBRARY_PATH="$BUILD/lib/src/x11:$BUILD/lib/ext/xext:$BUILD/lib/xcb:$BUILD/lib/src/xau:$BUILD/lib/src/xdmcp${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

"$XWM" -f "$ROOT/app/xwm/system.xwmrc" >"$LOG.xwm" 2>&1 &
XWM_PID=$!
sleep 0.5
if ! kill -0 "$XWM_PID" 2>/dev/null; then
  echo "xwm exited early; log:" >&2
  cat "$LOG.xwm" >&2 || true
  cat "$LOG" >&2 || true
  exit 1
fi

if ! "$BIN"; then
  echo "wm_manage failed; xwm log:" >&2
  cat "$LOG.xwm" >&2 || true
  cat "$LOG" >&2 || true
  exit 1
fi

echo "x12: xwm session OK"
