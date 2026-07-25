#!/usr/bin/env bash
# Non-Xvfb bring-up: Xephyr nested under Xvfb + X12-SURFACE QueryVersion (ADR-0016).
# Requires: meson setup with -Dxephyr=true && meson compile.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
XEPHYR="$BUILD/server/hw/kdrive/ephyr/Xephyr"
CC="${CC:-cc}"
PARENT="${X12_XEPHYR_PARENT:-94}"
NESTED="${X12_XEPHYR_NESTED:-95}"
PARENT_LOG=/tmp/x12-xephyr-parent.log
NESTED_LOG=/tmp/x12-xephyr-nested.log

need() { [[ -x "$1" ]] || { echo "missing $1 (build with -Dxephyr=true)" >&2; exit 1; }; }
need "$XVFB"
need "$XEPHYR"
[[ -x "$BUILD/app/xkbcomp/xkbcomp" || -x /usr/bin/xkbcomp ]] || {
  echo "missing xkbcomp (build app/xkbcomp or install x11-xkb-utils)" >&2
  exit 1
}

XCB_SO="$BUILD/lib/xcb/libxcb.so"
[[ -e "$XCB_SO" ]] || { echo "missing $XCB_SO" >&2; exit 1; }

BIN="$BUILD/tests/surface/query_version"
mkdir -p "$BUILD/tests/surface"
# shellcheck disable=SC2086
"$CC" -O2 -g ${CFLAGS:-} ${LDFLAGS:-} "$ROOT/tests/surface/query_version.c" -o "$BIN" \
  -I"$ROOT/include" -I"$BUILD" "$XCB_SO" -Wl,-rpath,"$BUILD/lib/xcb"

# Avoid stale displays from prior failed runs (common CI flake source).
rm -f "/tmp/.X${PARENT}-lock" "/tmp/.X${NESTED}-lock" \
  "/tmp/.X11-unix/X${PARENT}" "/tmp/.X11-unix/X${NESTED}"
mkdir -p /tmp/.X11-unix
chmod 1777 /tmp/.X11-unix 2>/dev/null || true

wait_socket() {
  local sock="$1" pid="$2" log="$3" label="$4" n="${5:-120}"
  local i
  for i in $(seq 1 "$n"); do
    [[ -S "$sock" ]] && return 0
    if [[ -n "$pid" ]] && ! kill -0 "$pid" 2>/dev/null; then
      echo "$label exited early" >&2
      [[ -f "$log" ]] && cat "$log" >&2 || true
      return 1
    fi
    sleep 0.1
  done
  echo "$label failed to create socket ($sock)" >&2
  [[ -f "$log" ]] && cat "$log" >&2 || true
  return 1
}

wait_xkb_ready() {
  local log="$1" n="${2:-100}"
  local i
  for i in $(seq 1 "$n"); do
    grep -q "Errors from xkbcomp are not fatal" "$log" 2>/dev/null && return 0
    sleep 0.1
  done
  # Not fatal: some builds omit that string; socket readiness is enough.
  return 0
}

export LD_LIBRARY_PATH="$BUILD/lib/src/x11:$BUILD/lib/xcb:$BUILD/lib/src/xau:$BUILD/lib/src/xdmcp:$BUILD/lib/ext/xext${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

"$XVFB" ":$PARENT" -ac -screen 0 1024x768x24 \
  -extension XFree86-Bigfont \
  -fp /usr/share/fonts/X11/misc,/usr/share/fonts/X11/Type1 \
  >"$PARENT_LOG" 2>&1 &
PARENT_PID=$!
XEPHYR_PID=""

cleanup() {
  [[ -n "${XEPHYR_PID}" ]] && kill "$XEPHYR_PID" 2>/dev/null || true
  kill "$PARENT_PID" 2>/dev/null || true
  rm -f "/tmp/.X${PARENT}-lock" "/tmp/.X${NESTED}-lock"
}
trap cleanup EXIT

wait_socket "/tmp/.X11-unix/X${PARENT}" "$PARENT_PID" "$PARENT_LOG" "parent Xvfb" 120 || exit 1
wait_xkb_ready "$PARENT_LOG" 100

export DISPLAY=":$PARENT"
unset XAUTHORITY

# Prefer software path; glamor can fail headless without a usable EGL config.
start_xephyr() {
  rm -f "/tmp/.X${NESTED}-lock" "/tmp/.X11-unix/X${NESTED}"
  : >"$NESTED_LOG"
  "$XEPHYR" ":$NESTED" -ac -screen 800x600x24 -sw-cursor \
    -extension XFree86-Bigfont \
    -fp /usr/share/fonts/X11/misc,/usr/share/fonts/X11/Type1 \
    >>"$NESTED_LOG" 2>&1 &
  XEPHYR_PID=$!
}

start_xephyr
if ! wait_socket "/tmp/.X11-unix/X${NESTED}" "$XEPHYR_PID" "$NESTED_LOG" "Xephyr" 200; then
  # One retry after cleaning locks (handles leftover display conflicts).
  echo "Xephyr: retrying after cleanup..." >&2
  kill "$XEPHYR_PID" 2>/dev/null || true
  wait "$XEPHYR_PID" 2>/dev/null || true
  XEPHYR_PID=""
  sleep 0.3
  start_xephyr
  wait_socket "/tmp/.X11-unix/X${NESTED}" "$XEPHYR_PID" "$NESTED_LOG" "Xephyr" 200 || {
    echo "--- parent Xvfb log ---" >&2
    cat "$PARENT_LOG" >&2 || true
    exit 1
  }
fi
wait_xkb_ready "$NESTED_LOG" 100

export DISPLAY=":$NESTED"
export LD_LIBRARY_PATH="$BUILD/lib/xcb${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
if ! "$BIN"; then
  echo "query_version failed; Xephyr log:" >&2
  cat "$NESTED_LOG" >&2 || true
  cat "$PARENT_LOG" >&2 || true
  exit 1
fi
echo "x12: xephyr QueryVersion OK"
