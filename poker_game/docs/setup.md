# 环境配置指南

本文档提供从零开始搭建 poker_game 开发环境的完整步骤，适用于 **Ubuntu 20.04+ / Debian 11+** 系统。

## 系统要求

- 操作系统：Ubuntu 20.04+ 或同版本 Debian
- 磁盘空间：约 5 GB（含 Cocos2d-x 引擎和构建产物）
- 网络：需要下载 Cocos2d-x 引擎依赖（首次配置时）

---

## 一、安装系统依赖

### 1. 更新包管理器

```bash
sudo apt-get update
```

### 2. 安装编译工具链

```bash
sudo apt-get install -y build-essential gcc g++ cmake git
```

> `build-essential` 包含 make、gcc、g++ 等基础编译工具。

### 3. 安装图形库依赖（OpenGL / GLFW / 音频）

```bash
sudo apt-get install -y \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libglew-dev \
    libglfw3-dev \
    libfreetype6-dev \
    libcurl4-openssl-dev \
    libogg-dev \
    libvorbis-dev \
    libopenal-dev \
    libx11-dev \
    libxxf86vm-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    pkg-config
```

> 这些是 Cocos2d-x 在 Linux 上运行所必需的系统库。

### 4. 验证安装

```bash
cmake --version   # 需要 >= 3.6
g++ --version     # 需要 C++17 支持
gcc --version
```

---

## 二、获取项目代码

### 方式 A：克隆仓库（推荐）

```bash
git clone <你的仓库地址> cocos_demo
cd cocos_demo
```

### 方式 B：使用已有代码

如果已下载或拷贝了项目，直接进入目录：

```bash
cd /your/path/to/cocos_demo
```

---

## 三、确认 Cocos2d-x 引擎

本项目已将 Cocos2d-x 3.17.2 引擎代码包含在 `poker_game/cocos2d/` 目录中，**无需单独下载引擎**。

确认引擎目录存在：

```bash
ls poker_game/cocos2d/
```

应看到 `cocos`、`external`、`tools` 等目录。如果不存在，请从其他渠道补齐。

---

## 四、构建与运行

### 4.1 使用一键脚本（推荐）

```bash
./run_poker_game.sh
```

脚本自动完成：CMake 配置 → 编译 → 启动游戏。

### 4.2 手动分步执行

```bash
# 步骤 1：生成构建文件
cmake -S poker_game -B poker_game/build

# 步骤 2：编译（-j4 表示 4 并行）
cmake --build poker_game/build -j4

# 步骤 3：运行
./poker_game/build/bin/poker_game/poker_game
```

### 4.3 Release 模式构建

```bash
BUILD_TYPE=Release ./run_poker_game.sh
```

或手动：

```bash
cmake -S poker_game -B poker_game/build -DCMAKE_BUILD_TYPE=Release
cmake --build poker_game/build -j$(nproc)
```

---

## 五、常见问题

| 问题 | 原因 | 解决方法 |
|------|------|---------|
| `cmake not found` | 未安装 CMake | 执行步骤一第 2 步 |
| `GL/gl.h: No such file` | 缺少 OpenGL 开发头文件 | 执行步骤一第 3 步 |
| `fatal error: glfw3.h` | 缺少 GLFW 库 | 执行步骤一第 3 步 |
| `C++17 required` | 编译器版本过低 | 升级 GCC 到 7+ |
| `cocos2d directory not found` | 引擎缺失 | 确认 `poker_game/cocos2d/` 存在 |

### 检查 GCC 版本是否支持 C++17

```bash
g++ --version | head -1
```

GCC 版本需 ≥ 7 才能完整支持 C++17。如果版本过低：

```bash
sudo apt-get install -y gcc-9 g++-9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90
```

---

## 六、可选：Android 构建（Docker）

如需打包 Android APK，还需安装 Docker：

```bash
# 安装 Docker（如果没有的话）
sudo apt-get install -y docker.io docker-compose

# 构建并输出 APK
./build_android_apk_docker.sh
```

APK 输出路径：`poker_game/proj.android/app/build/outputs/apk`
