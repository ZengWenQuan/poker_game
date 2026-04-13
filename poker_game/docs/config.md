# 配置系统 (config/)

## GlobalConfig

全局单例配置，启动时加载 3 个 JSON 文件，向所有模块提供统一的只读访问接口。

```cpp
// 字符串（支持多语言）
GlobalConfig::getInstance().get("undo");

// 窗口参数
GlobalConfig::getInstance().getFps();

// 颜色（RGBA）
GlobalConfig::getInstance().getColor4B("goldHighlight");

// 图片路径
GlobalConfig::getInstance().getImage("background");

// 对话框坐标/尺寸（带默认值）
GlobalConfig::getInstance().getDialogVec2("startOverlay", "titlePosition");
GlobalConfig::getInstance().getDialogSize("saveDialog", "panelSize");
GlobalConfig::getInstance().getDialogInt("saveDialog", "editBoxMaxLength");

// 编辑器参数（带默认值）
GlobalConfig::getInstance().getEditorFloat("trayMarginLeft", 60.0f);
GlobalConfig::getInstance().getEditorInt("trayColumns", 13);
```

## 配置文件

### game_config.json — 核心游戏配置

启动参数：

```json
"window": {
    "title": "poker_game",
    "designWidth": 1920,
    "designHeight": 1080,
    "scale": 0.45,
    "fps": 60,
    "showStats": true
}
```

卡牌视觉比例（内部布局）：

```json
"card": {
    "sourceWidth": 182,
    "sourceHeight": 282,
    "widthRatioToBackground": 0.05,
    "suitTargetWidthRatio": 0.18,
    "smallRankTargetHeightRatio": 0.14,
    "bigRankTargetHeightRatio": 0.40,
    "cornerInsetXRatio": 0.10,
    "cornerInsetYRatio": 0.06
}
```

游戏规则（核心可调参数）：

```json
"game": {
    "matchRankDiff": 1,
    "rewardCardsPerBonus": 3,
    "standardDeckSize": 52,
    "visibleTopCardCountMin": 1,
    "visibleTopCardCountMax": 3,
    "editorRewardCardCount": 4
}
```

| 字段 | 说明 | 默认值 |
|------|------|--------|
| `matchRankDiff` | 匹配点数差（1 表示差值为 1 可匹配） | 1 |
| `rewardCardsPerBonus` | 每张奖励牌消除时补充到底牌堆的牌数 | 3 |
| `standardDeckSize` | 标准牌组大小（用于计算需要几副牌） | 52 |
| `visibleTopCardCountMin` | 明牌窗口最小值（困难难度） | 1 |
| `visibleTopCardCountMax` | 明牌窗口最大值（简单难度） | 3 |
| `editorRewardCardCount` | 编辑器中奖励牌最大数量 | 4 |

交互参数：

```json
"interaction": {
    "doubleClickIntervalMs": 400,
    "dragThresholdPx": 10,
    "dropAreaPaddingRatio": 0.5,
    "reserveHitPadding": 20,
    "recycleHitRect": [-40, -20, 240, 60],
    "dragZOrder": 1000
}
```

动画时长：

```json
"animation": {
    "matchFlyDuration": 0.3,
    "flipDuration": 0.2,
    "bounceBackDuration": 0.25,
    "drawDelayDuration": 0.3,
    "undoFlyDuration": 0.3,
    "topCardScaleUpDuration": 0.2,
    "topCardScaleUpFrom": 0.8,
    "rewardFlyDelay": 0.15,
    "rewardFlyDuration": 0.25,
    "flyingCardZOrder": 10
}
```

UI 布局坐标：

```json
"ui": {
    "undoPosition": [120, 60],
    "selectedScale": 1.08,
    "cardSlotExtraHeight": 200
}
```

布局搜索路径：

```json
"layoutDirs": ["config/layouts/", "layouts/"],
"layouts": ["config/layouts/tripeaks.json", ...],
"defaultLayoutIndex": 0
```

### theme.json — 主题配置

字体与字号：

```json
"font": "fonts/wqy-microhei.ttc",
"fontSize": {
    "undo": 36, "status": 28, "levelTitle": 32,
    "levelBtn": 26, "recycle": 32, "reserveLabel": 28
}
```

颜色（RGBA `[r,g,b,a]`）：

```json
"colors": {
    "statusText": [255, 255, 200, 255],
    "goldHighlight": [255, 220, 100, 255],
    "winGold": [255, 215, 0, 255],
    "cardHighlight": [180, 255, 180],
    "cardGreyed": [160, 160, 160],
    "redSuit": [190, 40, 40, 255],
    "buttonText": [255, 255, 255, 255],
    "rewardCardFallback": [255, 204, 0, 255],
    "overlayMask": [0, 0, 0, 170],
    "dialogPanel": [26, 48, 30, 235],
    "dialogPanelAlt": [30, 45, 32, 235],
    "editorMainAreaMask": [255, 255, 255, 18],
    "editorTrayMask": [0, 0, 0, 60]
}
```

对话框尺寸与坐标（相对于面板左上角）：

```json
"dialogs": {
    "startOverlay": {
        "panelSize": [620, 280],
        "titlePosition": [310, 220],
        "difficultyOnePosition": [170, 118],
        "startButtonPosition": [310, 52]
    },
    "saveDialog": {
        "panelSize": [520, 220],
        "editBoxSize": [360, 56],
        "confirmPosition": [180, 42]
    },
    "confirmDialog": {
        "panelSize": [620, 240],
        "titlePosition": [310, 185]
    }
}
```

编辑器参数：

```json
"editor": {
    "trayMarginLeft": 60,
    "trayMarginBottom": 40,
    "trayWidthAdjust": -120,
    "trayHeightRatio": 0.28,
    "trayColumns": 13,
    "mainAreaGridDivisions": 100
}
```

图片路径（含模板）：

```json
"images": {
    "background": "res/background.png",
    "cardBack": "res/card_back.png",
    "cardFace": "res/card_general.png",
    "suitSpade": "res/suits/spade.png",
    "bigRankFormat": "res/number/big_{color}_{rank}.png"
}
```

模板路径通过 `getImageFormatted(key, color, rank)` 替换占位符。

### strings.json — 多语言字符串

```json
{
    "en": { "undo": "UNDO", "difficultyEasy": "EASY", ... },
    "zh": { "undo": "撤销", "difficultyEasy": "简单", ... }
}
```

`GlobalConfig::setLanguage("zh")` 切换语言，`get(key)` 自动取当前语言。

## JsonConfig

底层 JSON 解析工具，提供点分路径取值：

```cpp
JsonConfig cfg("config/game_config.json");
float width = cfg.getFloat("window.designWidth", 1920.0f);
Vec2 pos = cfg.getVec2("ui.undoPosition", {120, 60});
std::vector<std::string> files = cfg.getStringArray("layouts");
```

## 设计原则

- 所有运行时可调参数均通过 JSON 外化，代码不包含魔法值
- `GlobalConfig` 提供强类型访问器，参数变更时不影响调用方
- 颜色/字号/动画时长等可独立调整，互不干扰
