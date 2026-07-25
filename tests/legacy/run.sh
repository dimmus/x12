#!/usr/bin/env bash
# Legacy suite entry point — see docs/LEGACY_TESTS.md
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

if [[ ! -x build/x12-server && ! -x build/Xvfb && ! -x build/hw/vfb/Xvfb ]]; then
  echo "legacy: SKIP — X12/X11 server binary not built yet; corpus import pending."
  echo "legacy: policy is active; CI must fail once server targets exist and tests are wired."
  exit 0
fi

echo "legacy: server present but corpus runner not yet implemented" >&2
exit 1
