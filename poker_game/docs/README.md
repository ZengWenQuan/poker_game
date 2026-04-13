# Poker Game 项目文档

基于 Cocos2d-x 3.17.2 的纸牌消除游戏，C++17 编写，支持多平台。

## 文档目录

| 文档 | 说明 |
|------|------|
| [环境配置指南](../docs/setup.md) | 从零搭建开发环境（系统依赖、构建、运行、常见问题） |
| [架构设计](architecture.md) | 整体分层架构、模块职责与数据流 |
| [数据模型层](model.md) | CardTypes / PokerCard / CardDeck / CardSlot / GameState / LayoutConfig |
| [控制器层](controller.md) | GameController / MatchEngine / UndoManager / OperationHistory |
| [视图层](view.md) | PokerCardView / CardSlotView / MainAreaView / TopAreaView |
| [配置系统](config.md) | GlobalConfig / JsonConfig / JSON 配置文件格式 |
| [布局系统](layouts.md) | 布局 JSON 格式、动态覆盖关系计算、自定义布局编辑器 |
| [游戏规则](gameplay.md) | 匹配规则、操作流程、撤销机制、胜利条件 |
| [构建与运行](building.md) | CMake 构建、平台打包、Docker 构建 |

## 快速开始

```bash
# 构建
cmake -S poker_game -B poker_game/build
cmake --build poker_game/build -j4

# 运行
./run_poker_game.sh
```

## 项目结构概览

```
poker_game/
├── Classes/                    # C++ 源码
│   ├── AppDelegate.h/.cpp      # 应用入口（FPS/帧率由配置控制）
│   ├── CardTypes.h             # 枚举定义
│   ├── PokerCard.h/.cpp        # 扑克牌数据
│   ├── GameScene.h/.cpp        # 主场景（分文件实现）
│   ├── model/                  # 数据模型层
│   ├── controller/             # 控制器层
│   ├── presenter/              # 表现器层
│   ├── view/                  # 视图层
│   ├── config/                # 配置管理（所有参数由 JSON 驱动）
│   ├── editor/                # 自定义布局编辑器
│   ├── history/               # 操作历史
│   └── logging/               # 日志系统
├── Resources/
│   ├── config/                # JSON 配置
│   │   ├── game_config.json   # 游戏规则/动画/UI 参数
│   │   ├── theme.json         # 字体/颜色/编辑器/图片
│   │   ├── strings.json       # 多语言文案（en/zh）
│   │   └── layouts/           # 布局模板
│   ├── fonts/
│   └── res/                   # 图片资源
└── docker/                    # Android 构建
```
