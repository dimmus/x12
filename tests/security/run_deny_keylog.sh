#!/usr/bin/env bash
# G1: capability deny keylog — sandbox cannot QueryKeymap while full can.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
CC="${CC:-cc}"

need() { [[ -x "$1" ]] || { echo "missing $1" >&2; exit 1; }; }
need "$XVFB"
need "$BUILD/app/xkbcomp/xkbcomp"

BIN="$BUILD/tests/security/deny_keylog"
mkdir -p "$BUILD/tests/security"

INC=(-I"$ROOT/include" -I"$BUILD")
LIBS=(
  -L"$BUILD/lib/src/x11" -lX11
  -L"$BUILD/lib/ext/xtst" -lXtst
  -L"$BUILD/lib/ext/xi" -lXi
  -L"$BUILD/lib/xcb" -lxcb
  -L"$BUILD/lib/src/xau" -lXau
  -L"$BUILD/lib/src/xdmcp" -lXdmcp
  -L"$BUILD/lib/ext/xext" -lXext
  -Wl,-rpath,"$BUILD/lib/src/x11:$BUILD/lib/ext/xtst:$BUILD/lib/ext/xi:$BUILD/lib/xcb:$BUILD/lib/src/xau:$BUILD/lib/src/xdmcp:$BUILD/lib/ext/xext"
)

# shellcheck disable=SC2086
"$CC" -O2 -g ${CFLAGS:-} ${LDFLAGS:-} "$ROOT/tests/security/deny_keylog.c" -o "$BIN" "${INC[@]}" "${LIBS[@]}"

DISP_NUM=91
LOG=/tmp/x12-deny-keylog-xvfb.log
"$XVFB" ":$DISP_NUM" -ac -screen 0 800x600x24 \
  -extension XFree86-Bigfont \
  -fp /usr/share/fonts/X11/misc \
  -client-level full \
  -sandbox-clients 2 \
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
if ! "$BIN"; then
  echo "deny_keylog binary failed; Xvfb log:" >&2
  cat "$LOG" >&2 || true
  exit 1
fi
echo "security: deny_keylog OK"
