#!/usr/bin/env bash
# Xvfb smoke: X12-SURFACE QueryVersion via in-tree libxcb (ADR-0012).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
CC="${CC:-cc}"

need() { [[ -x "$1" ]] || { echo "missing $1" >&2; exit 1; }; }
need "$XVFB"
need "$BUILD/app/xkbcomp/xkbcomp"

BIN="$BUILD/tests/surface/query_version"
mkdir -p "$BUILD/tests/surface"

XCB_SO="$BUILD/lib/xcb/libxcb.so"
[[ -e "$XCB_SO" ]] || { echo "missing $XCB_SO" >&2; exit 1; }

INC=(-I"$ROOT/include" -I"$BUILD")
# shellcheck disable=SC2086
"$CC" -O2 -g ${CFLAGS:-} ${LDFLAGS:-} "$ROOT/tests/surface/query_version.c" -o "$BIN" "${INC[@]}" \
  "$XCB_SO" -Wl,-rpath,"$BUILD/lib/xcb"

DISP_NUM=96
LOG=/tmp/x12-surface-qv-xvfb.log
"$XVFB" ":$DISP_NUM" -ac -screen 0 640x480x24 \
  -extension XFree86-Bigfont \
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
  echo "query_version failed; Xvfb log:" >&2
  cat "$LOG" >&2 || true
  exit 1
fi
echo "surface: QueryVersion OK"
