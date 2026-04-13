# 构建与运行

## 环境配置

首次搭建开发环境，请参考 [环境配置指南](../docs/setup.md)，包含完整的系统依赖安装步骤。

## 依赖

- Cocos2d-x 3.17.2（已包含在 `cocos2d/` 子目录）
- CMake 3.6+
- C++17 编译器（GCC 7+）

## 本地构建（Linux）

### 方式一：一键脚本（推荐）

```bash
./run_poker_game.sh
```

脚本自动完成：CMake 配置 → 编译 → 定位可执行文件 → 启动游戏。

支持环境变量：
- `BUILD_TYPE` — 构建类型（默认 `Debug`）
- `JOBS` — 并行编译数（默认 `nproc`）

```bash
BUILD_TYPE=Release JOBS=8 ./run_poker_game.sh
```

### 方式二：手动分步

```bash
# 生成构建文件
cmake -S poker_game -B poker_game/build

# 编译
cmake --build poker_game/build -j4

# 运行
./poker_game/build/bin/poker_game/poker_game
```

### 方式三：指定构建类型

```bash
cmake -S poker_game -B poker_game/build -DCMAKE_BUILD_TYPE=Release
cmake --build poker_game/build -j$(nproc)
```

## Android APK 构建（Docker）

```bash
./build_android_apk_docker.sh
```

需要已安装 Docker 和 docker compose。

Docker 镜像包含：
- Android NDK 28
- JDK 8（Gradle 4.6 兼容）
- Android SDK Platform 28 / Build Tools 28.0.3

构建脚本 `docker/android/build-apk.sh` 自动执行：定位 SDK/NDK → 生成 `local.properties` → 执行 `gradlew assembleDebug`。

APK 输出路径：`poker_game/proj.android/app/build/outputs/apk`

## 跨平台

| 平台 | 工程目录 | 构建方式 |
|------|---------|---------|
| Linux | `proj.linux/` | CMake 原生支持 |
| Windows | `proj.win32/` | CMake 原生支持 |
| macOS / iOS | `proj.ios_mac/` | Xcode 工程 |
| Android | `proj.android/` | Gradle + NDK |

## 资源部署

Linux / Windows 构建时，CMake 自动将 `Resources/` 复制到可执行文件旁的 `Resources/` 目录。

自定义布局保存在 `FileUtils::getWritablePath()/layouts/` 目录，运行时自动添加为搜索路径（由配置 `layoutDirs` 指定）。

## 日志

每次启动生成日志文件：`writablePath/logs/game_YYYYMMDD_HHMMSS.log`

日志级别：DEBUG / INFO / WARN / ERROR

```cpp
GAME_LOG_INFO("Layout loaded: name=%s slots=%d", name.c_str(), count);
GAME_LOG_ERROR("Failed to load layout: %s", path.c_str());
```
