#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMPOSE_FILE="${SCRIPT_DIR}/poker_game/docker/android/docker-compose.yml"

if ! command -v docker >/dev/null 2>&1; then
    echo "docker is not installed."
    exit 1
fi

if docker compose version >/dev/null 2>&1; then
    COMPOSE_CMD=(docker compose)
elif command -v docker-compose >/dev/null 2>&1; then
    COMPOSE_CMD=(docker-compose)
else
    echo "docker compose is not available."
    exit 1
fi

"${COMPOSE_CMD[@]}" -f "${COMPOSE_FILE}" build android-apk-builder
"${COMPOSE_CMD[@]}" -f "${COMPOSE_FILE}" run --rm android-apk-builder

echo
echo "If the build succeeded, the APK is under:"
echo "${SCRIPT_DIR}/poker_game/proj.android/app/build/outputs/apk"
