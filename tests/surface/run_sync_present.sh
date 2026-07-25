#!/usr/bin/env bash
# X12-SURFACE ImportSyncobj + timeline Present + MSC CompleteNotify.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
CC="${CC:-cc}"

need() { [[ -x "$1" ]] || { echo "missing $1" >&2; exit 1; }; }
need "$XVFB"
[[ -x "$BUILD/app/xkbcomp/xkbcomp" || -x /usr/bin/xkbcomp ]] || {
  echo "missing xkbcomp (build app/xkbcomp or install x11-xkb-utils)" >&2
  exit 1
}

XCB_SO="$BUILD/lib/xcb/libxcb.so"
[[ -e "$XCB_SO" ]] || { echo "missing $XCB_SO" >&2; exit 1; }

BIN="$BUILD/tests/surface/sync_present"
mkdir -p "$BUILD/tests/surface"

INC=(-I"$ROOT/include" -I"$BUILD")
# shellcheck disable=SC2086
"$CC" -O2 -g ${CFLAGS:-} ${LDFLAGS:-} "$ROOT/tests/surface/sync_present.c" -o "$BIN" "${INC[@]}" \
  "$XCB_SO" -Wl,-rpath,"$BUILD/lib/xcb"

DISP_NUM=94
LOG=/tmp/x12-sync-present-xvfb.log
"$XVFB" ":$DISP_NUM" -ac -screen 0 800x600x24 \
  -extension XFree86-Bigfont \
  -fp /usr/share/fonts/X11/misc \
  >"$LOG" 2>&1 &
XVFB_PID=$!
cleanup() { kill "$XVFB_PID" 2>/dev/null || true; }
trap cleanup EXIT

for _ in $(seq 1 80); do
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

export DISPLAY=":$DISP_NUM"
unset XAUTHORITY
export LD_LIBRARY_PATH="$BUILD/lib/xcb${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
if ! "$BIN"; then
  echo "sync_present failed; Xvfb log:" >&2
  cat "$LOG" >&2 || true
  exit 1
fi
echo "surface: sync_present OK"
