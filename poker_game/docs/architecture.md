# 架构设计

## 分层架构

项目采用 MVC + Presenter 分层：

```
┌─────────────────────────────────────────────┐
│                GameScene                     │  场景编排层
│  (背景/UI/关卡选择/游戏回调/开局面板)          │
│  ┌─── SceneUIManager: 状态栏/UI显隐管理       │
└───────────────┬─────────────────────────────┘
                │
┌───────────────▼─────────────────────────────┐
│           GameplayPresenter                   │  表现层
│  (动画编排/输入锁/状态同步/结果处理)            │
│  通过 IGameplayView 接口操作视图，不依赖具体类   │
└──────┬──────────────────────┬────────────────┘
       │                      │
┌──────▼──────┐        ┌──────▼──────────────┐
│  IGameplayView│       │    Controller 层     │
│  GameplayView │       │  GameController      │
│  (组合视图接口)│       │  HighlightService    │
│       │      │       │  MatchEngine          │
│  ┌────▼────┐│       │  LayoutFlowController │
│  │ View 层  ││       │  UndoManager          │
│  │MainArea  ││       └──────┬────────────────┘
│  │TopArea   ││              │
│  │CardSlot  ││       ┌──────▼──────┐
│  │PokerCard ││       │  Model 层    │
│  │InputDetect│       │  GameState   │
│  └──────────┘│       │  CardDeck    │
└──────────────┘       │  CardSlot    │
                       │  LayoutConfig│
                       └──────────────┘
```

## 数据流

1. **用户操作** → `CardSlotView` / `TopAreaView` 触摸回调
2. **GameScene** → 转发到 `GameController`
3. **GameController** → 执行规则判断，返回 `GameActionResult`
4. **GameplayPresenter** → 通过 `IGameplayView` 接口编排动画和视图刷新
5. **GameplayView** → 委托 `MainAreaView` / `TopAreaView` 更新显示

## 文件组织

| 文件 | 职责 |
|------|------|
| `GameScene.cpp` | 场景 init / 析构 |
| `GameSceneUI.cpp` | 背景 / UI 按钮构建 |
| `GameSceneLevel.cpp` | 关卡选择 / 切换 |
| `GameSceneGameplay.cpp` | 游戏逻辑回调绑定 |
| `GameSceneStartOverlay.cpp` | 开局难度选择面板 |
| `GameSceneCustom.cpp` | 自定义布局模式切换 |

## 组件概览

### Controller 层

| 文件 | 职责 |
|------|------|
| `GameController.h/cpp` | 操作入口，协调 GameState 和 OperationHistory |
| `MatchEngine.h/cpp` | 匹配规则判定，点数差由配置控制 |
| `HighlightService.h/cpp` | 高亮计算服务 |
| `UndoManager.h/cpp` | 撤销管理 |
| `LayoutFlowController.h/cpp` | 关卡切换流程控制 |

### Presenter 层

| 文件 | 职责 |
|------|------|
| `GameplayPresenter.h/cpp` | 动画编排、输入锁、视图-模型状态同步 |
| `SceneChromePresenter.h/cpp` | UI 菜单/覆盖层/难度选择 |
| `IGameplayView.h` | 视图抽象接口，解耦 Presenter 与具体 View |
| `GameplayView.h/cpp` | 组合 MainAreaView + TopAreaView |
| `SceneUIManager.h/cpp` | UI 状态管理（状态栏、显隐控制） |

### View 层

| 文件 | 职责 |
|------|------|
| `PokerCardView.h/cpp` | 单张卡牌渲染，尺寸和布局比例由配置驱动 |
| `CardSlotView.h/cpp` | 卡槽容器，交互检测 |
| `MainAreaView.h/cpp` | 主牌区，布局与卡槽管理 |
| `TopAreaView.h/cpp` | 顶部牌区（底牌堆/开放牌/回收） |
| `InputDetector.h/cpp` | 拖拽/双击检测工具类 |

## 设计原则

- **极简**：每个类只做一件事，函数尽量短
- **配置驱动**：布局、图片路径、颜色、字体、游戏规则、动画参数等全部外置到 JSON，代码不包含魔法值
- **单点维护**：覆盖关系、尺寸比例等逻辑只在一处实现
- **规则与视图分离**：`model/` 和 `controller/` 只管规则，`view/` 只管显示
- **接口解耦**：Presenter 通过 `IGameplayView` 接口操作视图，不依赖具体 View 类
- **职责拆分**：大类拆为独立服务（HighlightService、InputDetector、SceneUIManager）
