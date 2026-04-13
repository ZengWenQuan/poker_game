# 视图层 (view/)

## PokerCardView

单张扑克牌的视觉表现，继承 `cocos2d::Node`。

### 创建

```cpp
static PokerCardView* create(const PokerCard& card, bool faceUp = true);
```

### 卡牌尺寸系统

卡牌尺寸通过 `refreshCardMetrics()` 在启动时自动计算：

1. 检测 `cardFace` 和 `background` 图片的实际像素尺寸
2. 按 `cardWidthRatioToBackground` 计算目标卡牌宽度
3. 算出缩放比例 `s_cardScale`

| 静态方法 | 说明 |
|---------|------|
| `getSourceCardSize()` | 原始图片尺寸 |
| `getCardSize()` / `getCardWidth()` / `getCardHeight()` | 缩放后尺寸 |
| `getCardScale()` | 缩放系数 |
| `refreshCardMetrics()` | 重新计算尺寸（启动时调用一次） |

### 卡牌内部布局比例

各元素尺寸通过 `theme.json` 配置读取（`card` 段），运行时通过 `GlobalConfig` 获取：

| 配置键 | 默认值 | 说明 |
|--------|--------|------|
| `card.suitTargetWidthRatio` | 0.18 | 花色图标占卡牌宽度比 |
| `card.smallRankTargetHeightRatio` | 0.14 | 右上角小数字高度比 |
| `card.bigRankTargetHeightRatio` | 0.40 | 中间大数字高度比 |
| `card.cornerInsetXRatio` | 0.10 | 角落 X 内缩比 |
| `card.cornerInsetYRatio` | 0.06 | 角落 Y 内缩比 |

正面布局：
- 底图：`cardFace` 图片缩放填充
- 左上角：花色图标
- 右上角：小号数字
- 正中间：大号数字

背面布局：
- `cardBack` 图片缩放填充

### 动画

| 方法 | 说明 |
|------|------|
| `flipToFaceUp(duration)` | 翻牌动画（缩小 → 换面 → 放大），时长由配置 `flipDuration` 控制 |
| `setHighlight(bool)` | 高亮/取消高亮（变色，由 `cardHighlight`/`cardGreyed` 配置） |

## CardSlotView

卡槽视图，管理该位置上所有牌的显示和交互。

### 交互

- **双击**：触发 `_onDoubleTap` 回调（用于匹配）
- **拖动**：按下后移动超过 `dragThresholdPx`（配置）触发，支持 dragStart/dragMove/dragEnd 三段回调
- **拖放回弹**：`animateTopCardBack(callback)` 将拖动的牌弹回原位，时长由 `bounceBackDuration` 配置

### 布局

卡槽内的牌按 `cardOverlapY`（配置）间距垂直叠放，Z-Order 按顺序递增。拖拽时 zOrder 由 `dragZOrder`（配置，默认 1000）覆盖。

卡槽额外高度由 `cardSlotExtraHeight`（配置，默认 200）控制。

## MainAreaView

主牌区视图，管理所有卡槽视图。

| 方法 | 说明 |
|------|------|
| `setupFromLayout(layout)` | 从 LayoutConfig 创建/刷新所有 CardSlotView |
| `updateAllSlots(slots)` | 根据数据刷新所有卡槽显示 |
| `updateHighlights(states)` | 更新卡牌高亮状态 |
| `getSlotView(index)` | 获取指定卡槽视图 |

卡槽位置由 `LayoutConfig::resolveSlotPosition` 计算，支持旋转（以左下角为圆心）。Z-Order 按 `maxLayer - slotLayer` 排列，确保上层牌遮挡下层。

## TopAreaView

顶部牌区视图，包含底牌堆、开放顶牌、回收按钮。

| 组件 | 说明 |
|------|------|
| `_reserveDeckView` | 底牌堆（多张背朝上的牌叠放） |
| `_reserveLabel` | 底牌/废牌数量文字 |
| `_recycleButton` | 回收按钮（底牌堆空且废牌堆非空时显示） |
| `_openTopCardViews` | 开放顶牌（数量 1~3，由难度决定） |

| 方法 | 说明 |
|------|------|
| `setOpenTopCards(cards)` | 设置开放顶牌 |
| `animateOpenTopCards(cards)` | 带缩放动画设置顶牌，动画时长由 `topCardScaleUpDuration` 控制，初始缩放由 `topCardScaleUpFrom` 控制 |
| `setReserveCount(count)` | 更新底牌数 |
| `setWastePileCount(count)` | 更新废牌数 |
| `getTopCardWorldRect()` | 获取顶牌区域世界矩形（拖放检测用） |

## InputDetector

从 CardSlotView 提取的拖拽/双击检测工具类。检测阈值由 `doubleClickIntervalMs`（配置）控制。
