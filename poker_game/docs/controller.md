# 控制器层 (controller/)

## MatchEngine

匹配规则引擎，纯静态工具类。

```cpp
static bool canMatch(const PokerCard& a, const PokerCard& b);
// 规则：两张牌点数差等于 matchRankDiff 即可匹配（不分花色）
// matchRankDiff 由配置 game.matchRankDiff 控制，默认值为 1

static std::pair<int, int> getMatchingRanks(const PokerCard& topCard);
// 返回可匹配的上下点数值，超出 1~13 范围返回 -1
```

## GameController

游戏操作的总入口，协调 `GameState` 和 `OperationHistory`。

### 操作接口

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `onSlotCardTapped(slotIndex)` | MatchSuccess / MatchFail | 点击主牌区卡牌，尝试与开放顶牌匹配 |
| `onDrawReserveCard()` | Draw | 从底牌堆抽牌 |
| `onRecycleWastePile()` | Recycle | 废牌堆回收到底牌堆 |
| `onUndo()` | UndoMatch / UndoDraw / NoUndo | 撤销上一步操作 |

### GameActionResult

```cpp
struct GameActionResult {
    GameActionType type;  // None / MatchSuccess / MatchFail / Draw / Recycle / UndoMatch / UndoDraw / NoUndo
    int slotIndex = -1;   // 涉及的卡槽
    PokerCard card;        // 涉及的牌
    bool won = false;      // 是否胜利
};
```

### 匹配流程

1. 检查 slot 有效且顶部牌翻开
2. 遍历 `_openTopCards`，用 `MatchEngine::canMatch` 检查（点数差由配置 `matchRankDiff` 控制）
3. 匹配成功：移除卡槽牌 → 记录操作历史 → 替换开放顶牌
4. 匹配失败：返回 MatchFail

### 查询接口

| 方法 | 说明 |
|------|------|
| `isSlotMatchable(index)` | 某卡槽的牌是否可匹配当前任一开放顶牌 |
| `getHighlightStates()` | 返回所有卡槽的可匹配状态（用于高亮显示） |

## HighlightService

从 GameController 提取的高亮计算服务，专注判断槽位是否可匹配。

## LayoutFlowController

关卡切换控制器，管理布局切换流程、加载、保存确认对话框等。

## OperationHistory (history/)

操作历史栈，用于撤销。

```cpp
enum class OperationType { Match, DrawCard };

struct GameOperation {
    OperationType type;
    PokerCard movedCard;                       // Match 移动的牌
    int slotIndex = -1;                        // 来源卡槽
    std::vector<int> revealedSlotIndices;      // Match 导致翻开的卡槽
    PokerCard drawnCard;                       // Draw 抽到的牌
    std::vector<PokerCard> previousOpenTopCards;  // 操作前的顶牌快照
    bool movedOpenCardToWaste = false;         // Draw 是否有旧顶牌被移入废牌堆
};
```

| 方法 | 说明 |
|------|------|
| `recordMatch(...)` | 记录一次匹配操作 |
| `recordDraw(...)` | 记录一次抽牌操作 |
| `pop()` | 弹出最近操作 |
| `canUndo()` | 是否可撤销 |
| `clear()` | 清空（回收废牌堆时调用） |
