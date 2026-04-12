#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="${PROJECT_ROOT:-/workspace/poker_game}"
ANDROID_PROJECT_DIR="${ANDROID_PROJECT_DIR:-${PROJECT_ROOT}/proj.android}"
SDK_DIR="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-}}"

if [[ -z "${SDK_DIR}" ]]; then
    for candidate in /opt/android-sdk /opt/android-sdk-linux /sdk; do
        if [[ -d "${candidate}" ]]; then
            SDK_DIR="${candidate}"
            break
        fi
    done
fi

if [[ -z "${SDK_DIR}" || ! -d "${SDK_DIR}" ]]; then
    echo "Android SDK directory not found."
    exit 1
fi

NDK_DIR="${ANDROID_NDK_HOME:-}"
if [[ -z "${NDK_DIR}" ]]; then
    for candidate in \
        "${SDK_DIR}/ndk-bundle" \
        "${SDK_DIR}/ndk/28.0.13004108" \
        "${SDK_DIR}/ndk"; do
        if [[ -d "${candidate}" ]]; then
            if [[ -f "${candidate}/ndk-build" ]]; then
                NDK_DIR="${candidate}"
                break
            fi
            latest_ndk="$(find "${candidate}" -mindepth 1 -maxdepth 1 -type d | sort | tail -n 1 || true)"
            if [[ -n "${latest_ndk}" && -f "${latest_ndk}/ndk-build" ]]; then
                NDK_DIR="${latest_ndk}"
                break
            fi
        fi
    done
fi

if [[ -z "${NDK_DIR}" || ! -d "${NDK_DIR}" ]]; then
    echo "Android NDK directory not found."
    exit 1
fi

if [[ ! -d "${ANDROID_PROJECT_DIR}" ]]; then
    echo "Android project directory not found: ${ANDROID_PROJECT_DIR}"
    exit 1
fi

export JAVA_HOME="${JAVA8_HOME:-/opt/jdk8}"
export PATH="${JAVA_HOME}/bin:${PATH}"
export ANDROID_HOME="${SDK_DIR}"
export ANDROID_SDK_ROOT="${SDK_DIR}"
export ANDROID_NDK_HOME="${NDK_DIR}"

cat > "${ANDROID_PROJECT_DIR}/local.properties" <<EOF
sdk.dir=${SDK_DIR}
ndk.dir=${NDK_DIR}
EOF

cd "${ANDROID_PROJECT_DIR}"
chmod +x gradlew

./gradlew --no-daemon :poker_game:assembleDebug

APK_PATH="$(find "${ANDROID_PROJECT_DIR}/app/build/outputs/apk" -type f -name '*.apk' | sort | tail -n 1 || true)"
if [[ -z "${APK_PATH}" ]]; then
    echo "Build finished, but no APK was found."
    exit 1
fi

echo "APK built successfully:"
echo "${APK_PATH}"
