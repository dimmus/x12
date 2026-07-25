#!/usr/bin/env bash
# Memory-safe protocol front-end tests + IDL drift gate (ROADMAP step 6 / ADR-0012).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

if ! command -v cargo >/dev/null 2>&1; then
  echo "safe_proto: cargo not found; install Rust toolchain" >&2
  exit 1
fi

echo "==> cargo test (safe/x12-proto)"
cargo test --manifest-path safe/x12-proto/Cargo.toml --locked

echo "==> IDL drift check (xcb sizeof vs x12_proto.h)"
cc -std=c11 -Wall -Werror \
  -Iinclude -Isafe/x12-proto/include \
  tests/safe_proto/drift_check.c \
  -o /tmp/x12-drift-check
/tmp/x12-drift-check

echo "safe_proto: PASSED"
