FROM saschpe/android-ndk:35-jdk23.0.2_7-ndk28.0.13004108-cmake3.31.5

# This project uses Gradle 4.6 + Android Gradle Plugin 3.1.0,
# which is much more reliable on JDK 8 than on the JDK bundled in modern Android images.
# Keep the Android SDK / NDK from the base image, but install a dedicated JDK 8
# and use it only for the Gradle build.

USER root

ENV DEBIAN_FRONTEND=noninteractive
ENV JDK8_DIR=/opt/jdk8
ENV JAVA8_HOME=/opt/jdk8
ENV GRADLE_USER_HOME=/workspace/.gradle

RUN set -eux; \
    if command -v apt-get >/dev/null 2>&1; then \
        apt-get update; \
        apt-get install -y --no-install-recommends bash wget tar unzip ca-certificates; \
        rm -rf /var/lib/apt/lists/*; \
    elif command -v apk >/dev/null 2>&1; then \
        apk add --no-cache bash wget tar unzip ca-certificates; \
    else \
        echo "Unsupported package manager"; \
        exit 1; \
    fi

RUN set -eux; \
    ARCH="$(uname -m)"; \
    case "${ARCH}" in \
        x86_64) JDK_ARCH="x64" ;; \
        aarch64) JDK_ARCH="aarch64" ;; \
        *) echo "Unsupported CPU architecture: ${ARCH}"; exit 1 ;; \
    esac; \
    JDK_URL="https://github.com/adoptium/temurin8-binaries/releases/download/jdk8u442-b06/OpenJDK8U-jdk_${JDK_ARCH}_linux_hotspot_8u442b06.tar.gz"; \
    mkdir -p /opt; \
    wget -O /tmp/jdk8.tar.gz "${JDK_URL}"; \
    tar -xzf /tmp/jdk8.tar.gz -C /opt; \
    mv /opt/jdk8u442-b06 "${JDK8_DIR}"; \
    rm -f /tmp/jdk8.tar.gz

RUN yes | sdkmanager --licenses >/dev/null

# Install the Android pieces this old cocos2d-x project expects.
RUN sdkmanager \
    "platform-tools" \
    "platforms;android-28" \
    "build-tools;28.0.3" \
    "cmake;3.10.2.4988404"

WORKDIR /workspace

COPY docker/android/build-apk.sh /usr/local/bin/build-apk.sh
RUN chmod +x /usr/local/bin/build-apk.sh

ENTRYPOINT ["/usr/local/bin/build-apk.sh"]
