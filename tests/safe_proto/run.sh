#!/usr/bin/env bash
# Memory-safe protocol front-end tests (ROADMAP step 6).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

if ! command -v cargo >/dev/null 2>&1; then
  echo "safe_proto: cargo not found; install Rust toolchain" >&2
  exit 1
fi

echo "==> cargo test (safe/x12-proto)"
cargo test --manifest-path safe/x12-proto/Cargo.toml --locked

echo "safe_proto: PASSED"
