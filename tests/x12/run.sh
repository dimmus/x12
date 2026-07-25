#!/usr/bin/env bash
# X12-native G1 smokes (not part of the locked X11R8 B2 corpus).
# Called from tests/legacy/run.sh after the corpus, and directly from CI.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"
FAIL=0

echo "x12: security deny_keylog"
if ! "$ROOT/tests/security/run_deny_keylog.sh"; then
  echo "x12: FAIL — deny_keylog" >&2
  FAIL=1
fi

echo "x12: security deny_matrix"
if ! "$ROOT/tests/security/run_deny_matrix.sh"; then
  echo "x12: FAIL — deny_matrix" >&2
  FAIL=1
fi

echo "x12: surface QueryVersion"
if ! "$ROOT/tests/surface/run_query_version.sh"; then
  echo "x12: FAIL — surface QueryVersion" >&2
  FAIL=1
fi

echo "x12: surface vk_present"
if ! "$ROOT/tests/surface/run_vk_present.sh"; then
  echo "x12: FAIL — surface vk_present" >&2
  FAIL=1
fi

echo "x12: surface sync_present"
if ! "$ROOT/tests/surface/run_sync_present.sh"; then
  echo "x12: FAIL — surface sync_present" >&2
  FAIL=1
fi

echo "x12: surface scale_stub"
if ! "$ROOT/tests/surface/run_scale_stub.sh"; then
  echo "x12: FAIL — surface scale_stub" >&2
  FAIL=1
fi

if [[ "$FAIL" -ne 0 ]]; then
  echo "x12: FAILED" >&2
  exit 1
fi
echo "x12: PASSED"
exit 0
