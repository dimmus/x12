#!/usr/bin/env bash
# Non-Xvfb bring-up: Xephyr nested under Xvfb + X12-SURFACE QueryVersion (ADR-0016).
# Requires: meson setup with -Dxephyr=true && meson compile.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
XEPHYR="$BUILD/server/hw/kdrive/ephyr/Xephyr"
CC="${CC:-cc}"

need() { [[ -x "$1" ]] || { echo "missing $1 (build with -Dxephyr=true)" >&2; exit 1; }; }
need "$XVFB"
need "$XEPHYR"

XCB_SO="$BUILD/lib/xcb/libxcb.so"
[[ -e "$XCB_SO" ]] || { echo "missing $XCB_SO" >&2; exit 1; }

BIN="$BUILD/tests/surface/query_version"
mkdir -p "$BUILD/tests/surface"
"$CC" -O2 -g "$ROOT/tests/surface/query_version.c" -o "$BIN" \
  -I"$ROOT/include" -I"$BUILD" "$XCB_SO" -Wl,-rpath,"$BUILD/lib/xcb"

PARENT=94
NESTED=95
XEPHYR_PID=""

"$XVFB" ":$PARENT" -ac -screen 0 1024x768x24 \
  -extension XFree86-Bigfont \
  >/tmp/x12-xephyr-parent.log 2>&1 &
PARENT_PID=$!

cleanup() {
  [[ -n "${XEPHYR_PID}" ]] && kill "$XEPHYR_PID" 2>/dev/null || true
  kill "$PARENT_PID" 2>/dev/null || true
}
trap cleanup EXIT

for _ in $(seq 1 40); do
  [[ -S "/tmp/.X11-unix/X${PARENT}" ]] && break
  sleep 0.1
done
[[ -S "/tmp/.X11-unix/X${PARENT}" ]] || {
  cat /tmp/x12-xephyr-parent.log >&2
  exit 1
}

export DISPLAY=":$PARENT"
unset XAUTHORITY
"$XEPHYR" ":$NESTED" -ac -screen 800x600 \
  -extension XFree86-Bigfont \
  >/tmp/x12-xephyr-nested.log 2>&1 &
XEPHYR_PID=$!

for _ in $(seq 1 50); do
  [[ -S "/tmp/.X11-unix/X${NESTED}" ]] && break
  sleep 0.1
done
[[ -S "/tmp/.X11-unix/X${NESTED}" ]] || {
  echo "Xephyr failed" >&2
  cat /tmp/x12-xephyr-nested.log >&2
  exit 1
}

export DISPLAY=":$NESTED"
export LD_LIBRARY_PATH="$BUILD/lib/xcb${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
"$BIN"
echo "x12: xephyr QueryVersion OK"
