# 数据模型层 (model/)

## CardTypes.h

枚举定义，全局共享：

```cpp
enum class CardSuit  { Spade, Heart, Club, Diamond };
enum class CardRank  { Ace = 1, Two, ..., Ten, Jack, Queen, King };
```

## PokerCard

扑克牌数据模型，轻量值对象。

| 方法 | 说明 |
|------|------|
| `isRedSuit()` | Heart/Diamond 为红色 |
| `rankToken()` | 返回点数字符串 "A"/"2"~"10"/"J"/"Q"/"K" |
| `suitTexturePath()` | 从 GlobalConfig 取花色贴图路径 |
| `bigRankTexturePath()` | 大号数字贴图路径（含 `{color}`/`{rank}` 模板替换） |
| `smallRankTexturePath()` | 小号数字贴图路径 |
| `rankValue()` | 返回 1~13 的数值 |
| `isReward()` | 是否为奖励牌（金色特殊牌） |
| `operator==` / `!=` | 比较花色和点数 |

## CardDeck

牌堆，支持多副牌。牌组大小由 `GlobalConfig::getStandardDeckSize()` 配置（默认 52）。

| 方法 | 说明 |
|------|------|
| `createMultipleStandardDecks(count)` | 创建 count 副标准牌 |
| `shuffle()` | 随机洗牌 |
| `dealCard()` | 从顶部发一张 |
| `isEmpty()` / `size()` | 查询状态 |
| `reset(cards)` / `addCardToTop(card)` | 重置 / 追加 |

## CardSlot

单个卡槽，存储该位置的牌堆。

| 方法 | 说明 |
|------|------|
| `pushCard(card, faceUp)` | 放入一张牌 |
| `popCard()` | 取出顶部牌 |
| `topCard()` | 查看顶部牌 |
| `isTopCardFaceUp()` / `setTopCardFaceUp()` | 翻牌状态 |
| `isEmpty()` / `cardCount()` | 查询状态 |
| `getCard(index)` | 按索引查看 |

内部用 `SlotCard { PokerCard card; bool faceUp; }` 存储。

## GameState

游戏全局状态，核心数据源。

### 成员

| 成员 | 说明 |
|------|------|
| `_layoutConfig` | 当前布局配置 |
| `_mainAreaSlots` | 主牌区所有卡槽 |
| `_coveringParents` / `_coveredChildren` | 覆盖关系（邻接表） |
| `_reserveDeck` | 底牌堆 |
| `_openTopCards` | 开放顶牌（数量由难度决定，范围由配置控制） |
| `_wastePile` | 废牌堆 |

### 关键方法

| 方法 | 说明 |
|------|------|
| `loadLayout(path)` | 加载布局 JSON，构建覆盖关系 |
| `initializeGame()` | 洗牌、发牌到卡槽和底牌堆 |
| `drawFromReserve()` | 从底牌堆抽一张 |
| `recycleWastePile()` | 废牌堆洗牌回底牌堆 |
| `removeCardFromSlot(index, revealedIndices)` | 从卡槽移除牌，自动翻开下层牌 |
| `restoreCardToSlot(index, card, revealedIndices)` | 撤销时恢复牌 |
| `isSlotExposed(index)` | 判断卡槽是否暴露 |
| `isWin()` | 所有卡槽为空则胜利 |

### 初始化逻辑

1. 根据 `_layoutConfig` 的 slot 数量创建卡槽
2. 计算需要几副牌（由配置 `standardDeckSize` 决定）
3. 洗牌后依次发牌到每个卡槽
4. 没有被任何牌压着的卡槽 → 牌面朝上
5. 额外发 `visibleTopCardCount` 张到开放顶牌区（范围由配置 `visibleTopCardCountMin/Max` 控制）
6. 剩余牌作为底牌堆

## LayoutConfig

布局配置，详见 [布局系统](layouts.md)。

搜索路径由配置 `layoutDirs` 指定（默认 `config/layouts/` 和 `layouts/`），优先级从前到后。
