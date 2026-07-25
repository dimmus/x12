#!/usr/bin/env bash
# Non-Xvfb bring-up: Xephyr nested under Xvfb + X12-SURFACE QueryVersion (ADR-0016).
# Requires: meson setup with -Dxephyr=true && meson compile.
#
# Readiness uses the server -displayfd protocol (NotifyParentProcess), not a
# timed poll on /tmp/.X11-unix — socket presence alone races Xvfb/Xephyr init
# and was a CI flake source ("Xephyr failed to create socket").
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
XEPHYR="$BUILD/server/hw/kdrive/ephyr/Xephyr"
CC="${CC:-cc}"
PARENT_LOG=/tmp/x12-xephyr-parent.log
NESTED_LOG=/tmp/x12-xephyr-nested.log
WORKDIR=""
PARENT_PID=""
XEPHYR_PID=""
PARENT=""
NESTED=""

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

mkdir -p /tmp/.X11-unix
chmod 1777 /tmp/.X11-unix 2>/dev/null || true

pick_display() {
  local d
  local avoid_a="${1:-}"
  local avoid_b="${2:-}"
  for d in $(seq "${X12_XEPHYR_DISPLAY_BASE:-90}" 119); do
    [[ "$d" == "$avoid_a" || "$d" == "$avoid_b" ]] && continue
    if [[ ! -e "/tmp/.X${d}-lock" && ! -e "/tmp/.X11-unix/X${d}" ]]; then
      printf '%s\n' "$d"
      return 0
    fi
  done
  echo "no free X display in range" >&2
  return 1
}

PARENT="${X12_XEPHYR_PARENT:-$(pick_display)}"
NESTED="${X12_XEPHYR_NESTED:-$(pick_display "$PARENT")}"
# Clear stale artifacts for the chosen displays (common CI flake source).
rm -f "/tmp/.X${PARENT}-lock" "/tmp/.X${NESTED}-lock" \
  "/tmp/.X11-unix/X${PARENT}" "/tmp/.X11-unix/X${NESTED}"

WORKDIR=$(mktemp -d /tmp/x12-xephyr.XXXXXX)
PARENT_FIFO="$WORKDIR/parent.fd"
NESTED_FIFO="$WORKDIR/nested.fd"
mkfifo "$PARENT_FIFO" "$NESTED_FIFO"

cleanup() {
  [[ -n "${XEPHYR_PID}" ]] && kill "$XEPHYR_PID" 2>/dev/null || true
  [[ -n "${PARENT_PID}" ]] && kill "$PARENT_PID" 2>/dev/null || true
  [[ -n "${NESTED}" ]] && rm -f "/tmp/.X${NESTED}-lock" "/tmp/.X11-unix/X${NESTED}"
  [[ -n "${PARENT}" ]] && rm -f "/tmp/.X${PARENT}-lock" "/tmp/.X11-unix/X${PARENT}"
  [[ -n "${WORKDIR}" && -d "${WORKDIR}" ]] && rm -rf "$WORKDIR"
}
trap cleanup EXIT

export LD_LIBRARY_PATH="$BUILD/lib/src/x11:$BUILD/lib/xcb:$BUILD/lib/src/xau:$BUILD/lib/src/xdmcp:$BUILD/lib/ext/xext${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

# Open FIFOs RDWR so open() does not block before the peer exists (Linux).
exec {PARENT_FD}<>"$PARENT_FIFO"
exec {NESTED_FD}<>"$NESTED_FIFO"

read_displayfd() {
  # $1 = bash FD number, $2 = server pid, $3 = log, $4 = label
  # $5 = expected display number, $6 = timeout seconds
  local fd="$1" pid="$2" log="$3" label="$4" expect="$5" timeout_s="${6:-30}"
  local line=""
  local i
  for i in $(seq 1 "$((timeout_s * 10))"); do
    if read -r -t 0.1 line <&"$fd"; then
      line="${line//$'\r'/}"
      if [[ "$line" =~ ^[0-9]+$ ]]; then
        if [[ -n "$expect" && "$line" != "$expect" ]]; then
          echo "$label: displayfd reported :$line, expected :$expect" >&2
          [[ -f "$log" ]] && cat "$log" >&2 || true
          return 1
        fi
        printf '%s\n' "$line"
        return 0
      fi
      echo "$label: unexpected displayfd payload: [$line]" >&2
      [[ -f "$log" ]] && cat "$log" >&2 || true
      return 1
    fi
    if [[ -n "$pid" ]] && ! kill -0 "$pid" 2>/dev/null; then
      echo "$label exited before displayfd readiness" >&2
      [[ -f "$log" ]] && cat "$log" >&2 || true
      return 1
    fi
  done
  echo "$label failed to signal readiness via displayfd (${timeout_s}s)" >&2
  [[ -f "$log" ]] && cat "$log" >&2 || true
  return 1
}

: >"$PARENT_LOG"
# Explicit display + displayfd: bind a known free number, wait until accept-ready.
"$XVFB" ":$PARENT" -displayfd "$PARENT_FD" -ac -screen 0 1024x768x24 \
  -extension XFree86-Bigfont \
  -fp /usr/share/fonts/X11/misc,/usr/share/fonts/X11/Type1 \
  >"$PARENT_LOG" 2>&1 &
PARENT_PID=$!

read_displayfd "$PARENT_FD" "$PARENT_PID" "$PARENT_LOG" "parent Xvfb" "$PARENT" 30 >/dev/null || exit 1
export DISPLAY=":$PARENT"
unset XAUTHORITY

start_xephyr() {
  rm -f "/tmp/.X${NESTED}-lock" "/tmp/.X11-unix/X${NESTED}"
  : >"$NESTED_LOG"
  # Drain any stale byte from a previous attempt on the RDWR FIFO.
  while read -r -t 0 _ <&"$NESTED_FD"; do :; done || true
  "$XEPHYR" ":$NESTED" -displayfd "$NESTED_FD" -ac -screen 800x600x24 -sw-cursor \
    -extension XFree86-Bigfont \
    -fp /usr/share/fonts/X11/misc,/usr/share/fonts/X11/Type1 \
    >"$NESTED_LOG" 2>&1 &
  XEPHYR_PID=$!
}

start_xephyr
if ! read_displayfd "$NESTED_FD" "$XEPHYR_PID" "$NESTED_LOG" "Xephyr" "$NESTED" 45 >/dev/null; then
  echo "Xephyr: retrying once..." >&2
  kill "$XEPHYR_PID" 2>/dev/null || true
  wait "$XEPHYR_PID" 2>/dev/null || true
  XEPHYR_PID=""
  # Re-pick nested display in case the previous number is wedged.
  NESTED="${X12_XEPHYR_NESTED:-$(pick_display "$PARENT" "$NESTED")}"
  sleep 0.3
  start_xephyr
  read_displayfd "$NESTED_FD" "$XEPHYR_PID" "$NESTED_LOG" "Xephyr" "$NESTED" 45 >/dev/null || {
    echo "--- parent Xvfb log ---" >&2
    cat "$PARENT_LOG" >&2 || true
    exit 1
  }
fi

export DISPLAY=":$NESTED"
export LD_LIBRARY_PATH="$BUILD/lib/xcb${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
if ! "$BIN"; then
  echo "query_version failed; Xephyr log:" >&2
  cat "$NESTED_LOG" >&2 || true
  echo "--- parent Xvfb log ---" >&2
  cat "$PARENT_LOG" >&2 || true
  exit 1
fi
echo "x12: xephyr QueryVersion OK (parent=:$PARENT nested=:$NESTED)"
