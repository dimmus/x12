#!/usr/bin/env bash
# Fuzz gate for Rust X12-SURFACE decode (ADR-0016).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD="${X12_BUILD_DIR:-$ROOT/build}"
CC="${CC:-cc}"

# Regenerate wire constants from XML
python3 "$ROOT/safe/x12-proto/tools/gen_wire.py"

# Ensure staticlib exists
if [[ ! -f "$BUILD/safe/libx12_proto.a" ]]; then
  echo "fuzz: building libx12_proto via meson..."
  meson compile -C "$BUILD" 2>/dev/null || true
fi
LIB="$BUILD/safe/cargo-target/release/libx12_proto.a"
[[ -f "$LIB" ]] || LIB="$BUILD/safe/libx12_proto.a"
[[ -f "$LIB" ]] || {
  echo "fuzz: missing libx12_proto.a — run meson compile" >&2
  find "$BUILD" -name 'libx12_proto.a' 2>/dev/null | head
  exit 1
}

BIN="$BUILD/tests/safe_proto/fuzz_harness"
mkdir -p "$(dirname "$BIN")"
"$CC" -O2 -g "$ROOT/tests/safe_proto/fuzz_harness.c" -o "$BIN" \
  -I"$ROOT/safe/x12-proto/include" "$LIB" -lm -ldl -lpthread

ITERS="${X12_FUZZ_ITERS:-20000}"
"$BIN" "$ITERS"
echo "safe_proto: fuzz OK"
