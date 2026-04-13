/**
 * @file GameController.h
 * @brief 游戏控制器头文件。
 *
 * 主要功能:
 *   - handleReserveClick / handleMainPileClick / handleRecycleClick
 *   - undo() / redo()
 *   - checkWin() / advanceToNextLevel()
 */
#ifndef POKER_GAME_GAME_CONTROLLER_H
#define POKER_GAME_GAME_CONTROLLER_H

#include "GameState.h"
#include "MatchEngine.h"
#include "HighlightService.h"
#include "history/OperationHistory.h"

enum class GameActionType
{
    None,
    MatchSuccess, // 主牌区点击后成功匹配
    MatchFail,    // 点击了可见牌，但没有任何顶部明牌可匹配
    Draw,         // 从底牌堆抽一张牌进入顶部窗口
    Recycle,      // 废牌堆回收到底牌堆
    UndoMatch,    // 撤销一次匹配
    UndoDraw,     // 撤销一次抽牌
    NoUndo        // 当前没有可撤销操作
};

struct GameActionResult
{
    GameActionType type = GameActionType::None; // 本次输入对应的结果类型
    int slotIndex = -1;                         // 若和主牌区相关，则返回对应槽位索引
    PokerCard card;                             // 本次操作涉及的主要牌对象
    bool won = false;                           // 操作完成后是否已经获胜
    int rewardSlotIndex = -1;                    // 触发的奖励牌槽位索引，-1 表示无奖励牌
};

// 游戏流程控制器：封装匹配规则、抽牌/回收/撤销逻辑，直接操作 GameState。
class GameController
{
public:
    // 以游戏状态引用构造控制器。
    // state: 持久的 GameState，控制器直接读写其中数据。
    explicit GameController(GameState& state);

    // 开新局时同时重置状态与撤销历史。
    void startGame();

    // 处理主牌区点击：检查可见性、匹配规则、状态变更和历史记录。
    GameActionResult onSlotCardTapped(int slotIndex);

    // 处理顶部牌区的抽牌、回收和撤销流程。
    // 返回包含本次动作类型、涉及牌槽或牌的摘要。
    GameActionResult onDrawReserveCard();
    GameActionResult onRecycleWastePile();
    GameActionResult onUndo();

    // 获取当前可见的顶部明牌集合。
    const std::vector<PokerCard>& getOpenTopCards() const;

    // 高亮计算委托给 HighlightService。
    const HighlightService& getHighlightService() const;

private:
    GameState& _state;                  // 游戏状态唯一数据源
    HighlightService _highlightService; // 高亮计算服务
    OperationHistory _operationHistory; // 撤销历史
};

#endif
