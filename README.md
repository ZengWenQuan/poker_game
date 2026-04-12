# Poker Game — 纸牌消除游戏

基于 Cocos2d-x 3.17.2 的纸牌消除游戏，C++17 编写。

## 玩法简介

主牌区的牌按布局排列、层层覆盖，玩家需要通过**匹配消除**清空所有主牌：

- **匹配**：双击或拖动主牌区翻开的牌，若与开放顶牌点数差为 1 即可消除
- **抽牌**：点击底牌堆抽一张新顶牌
- **回收**：底牌耗尽后可将废牌堆洗牌回底牌堆
- **撤销**：随时撤回上一步操作

| 难度 | 开放顶牌数 |
|------|-----------|
| 困难 | 1 |
| 普通 | 2 |
| 简单 | 3 |

## 内置布局

| 布局 | 牌堆数 | 卡槽数 | 说明 |
|------|--------|--------|------|
| TriPeaks | 3 | 28 | 三峰金字塔 |
| Fan | 3 | 10 | 扇形展开 |
| Diamond | 2 | 15 | 菱形 |
| Spiral | 2 | 21 | 螺旋形 |

支持自定义布局编辑器，拖拽即可创建并保存新布局。

## 快速开始

```bash
# 构建
cmake -S poker_game -B poker_game/build
cmake --build poker_game/build -j4

# 运行
./run_poker_game.sh
```

## 项目结构

```
poker_game/
├── Classes/                  # C++ 源码
│   ├── AppDelegate.h/.cpp    # 应用入口
│   ├── CardTypes.h           # 枚举定义
│   ├── PokerCard.h/.cpp      # 扑克牌数据
│   ├── GameScene.h/.cpp      # 主场景
│   ├── model/                # GameState / CardDeck / CardSlot / LayoutConfig
│   ├── controller/           # GameController / MatchEngine / UndoManager
│   ├── presenter/            # GameplayPresenter（动画编排）
│   ├── view/                 # PokerCardView / CardSlotView / MainAreaView / TopAreaView
│   ├── config/               # JsonConfig / GlobalConfig
│   ├── editor/               # 自定义布局编辑器
│   ├── history/              # 操作历史
│   └── logging/              # 日志系统
├── Resources/
│   ├── config/               # game_config.json / theme.json / strings.json
│   ├── config/layouts/       # 布局模板 JSON
│   ├── fonts/
│   └── res/                  # 图片资源
├── docs/                     # 详细文档
└── docker/                   # Android 构建
```

## 架构

采用 **MVC + Presenter** 分层：

```
GameScene → GameplayPresenter → View + Controller → Model
```

- **Model**：牌局状态、牌堆、卡槽、布局配置
- **Controller**：匹配规则、操作入口、撤销管理
- **Presenter**：动画编排、输入锁、状态同步
- **View**：卡牌渲染、交互反馈

## 详细文档

| 文档 | 说明 |
|------|------|
| [架构设计](poker_game/docs/architecture.md) | 分层架构与数据流 |
| [数据模型层](poker_game/docs/model.md) | CardTypes / PokerCard / CardDeck / CardSlot / GameState / LayoutConfig |
| [控制器层](poker_game/docs/controller.md) | GameController / MatchEngine / UndoManager |
| [视图层](poker_game/docs/view.md) | 卡牌与区域视图 |
| [配置系统](poker_game/docs/config.md) | JSON 配置管理 |
| [布局系统](poker_game/docs/layouts.md) | 布局格式、动态覆盖、自定义编辑器 |
| [游戏规则](poker_game/docs/gameplay.md) | 匹配/抽牌/回收/撤销/拖放规则 |
| [构建与运行](poker_game/docs/building.md) | CMake 构建、Android 打包 |
