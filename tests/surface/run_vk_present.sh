#!/usr/bin/env bash
# G1: Vulkan (lavapipe) → memfd → X12-SURFACE Present under Xvfb.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
XVFB="$BUILD/server/hw/vfb/Xvfb"
CC="${CC:-cc}"

need() { [[ -x "$1" ]] || { echo "missing $1" >&2; exit 1; }; }
need "$XVFB"
need "$BUILD/app/xkbcomp/xkbcomp"

XCB_SO="$BUILD/lib/xcb/libxcb.so"
[[ -e "$XCB_SO" ]] || { echo "missing $XCB_SO" >&2; exit 1; }

BIN="$BUILD/tests/surface/vk_present"
mkdir -p "$BUILD/tests/surface"

INC=(-I"$ROOT/include" -I"$BUILD")
# shellcheck disable=SC2086
"$CC" -O2 -g ${CFLAGS:-} ${LDFLAGS:-} "$ROOT/tests/surface/vk_present.c" -o "$BIN" "${INC[@]}" \
  "$XCB_SO" -lvulkan -Wl,-rpath,"$BUILD/lib/xcb"

DISP_NUM=93
LOG=/tmp/x12-vk-present-xvfb.log
export VK_ICD_FILENAMES="${VK_ICD_FILENAMES:-/usr/share/vulkan/icd.d/lvp_icd.json}"

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
  echo "vk_present binary failed; Xvfb log:" >&2
  cat "$LOG" >&2 || true
  if [[ "${X12_ALLOW_CPU_PRESENT:-0}" == "1" ]]; then
    echo "vk_present: trying --cpu fallback" >&2
    "$BIN" --cpu
  else
    echo "vk_present: FAIL — Vulkan/lavapipe required (set X12_ALLOW_CPU_PRESENT=1 to override)" >&2
    exit 1
  fi
fi

echo "surface: vk_present OK"
