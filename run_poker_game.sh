#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/poker_game"
BUILD_DIR="$PROJECT_DIR/build"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

if [[ ! -d "$PROJECT_DIR" ]]; then
  echo "❌ Project directory not found: $PROJECT_DIR" >&2
  exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "❌ cmake is required but was not found in PATH." >&2
  exit 1
fi

# --- Build ---
echo "🔨 Building poker_game ($BUILD_TYPE) ..."
mkdir -p "$BUILD_DIR"

cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" 2>&1 | tail -5
cmake --build "$BUILD_DIR" -j"${JOBS:-$(nproc)}" 2>&1 | tail -5

# --- Locate binary ---
APP_BIN=""
for candidate in \
  "$BUILD_DIR/bin/poker_game/poker_game" \
  "$BUILD_DIR/bin/poker_game" \
  "$BUILD_DIR/poker_game" \
  "$(find "$BUILD_DIR" -maxdepth 4 -type f -name poker_game -perm -111 2>/dev/null | head -n 1)"
do
  if [[ -n "${candidate:-}" && -f "$candidate" && -x "$candidate" ]]; then
    APP_BIN="$candidate"
    break
  fi
done

if [[ -z "$APP_BIN" ]]; then
  echo "❌ Build finished, but poker_game executable not found under $BUILD_DIR." >&2
  exit 1
fi

echo "🚀 Launching poker_game ..."
cd "$(dirname "$APP_BIN")"
exec "$APP_BIN" "$@"
