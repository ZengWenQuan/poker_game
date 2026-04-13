/**
 * @file OperationHistory.cpp
 * @brief 操作历史记录实现。
 *
 * 主要功能:
 *   - recordMatch: 记录匹配操作
 *   - recordRecycle: 记录回收操作
 *   - clear: 清空历史
 */
#include "history/OperationHistory.h"

// 记录一次匹配操作以供撤销。
void OperationHistory::recordMatch(const PokerCard& movedCard,
                                   int slotIndex,
                                   const std::vector<int>& revealedSlotIndices,
                                   const std::vector<PokerCard>& previousOpenTopCards,
                                   const std::vector<RewardGrant>& rewardGrants)
{
    // 撤销匹配至少要恢复顶部窗口、来源槽位和被翻开的子节点。
    GameOperation operation;
    operation.type = OperationType::Match;
    operation.movedCard = movedCard;
    operation.slotIndex = slotIndex;
    operation.revealedSlotIndices = revealedSlotIndices;
    operation.previousOpenTopCards = previousOpenTopCards;
    operation.rewardGrants = rewardGrants;
    _operations.push_back(operation);
}

// 记录一次抽牌操作以供撤销。
void OperationHistory::recordDraw(const PokerCard& drawnCard,
                                  const std::vector<PokerCard>& previousOpenTopCards,
                                  bool movedOpenCardToWaste)
{
    // 抽牌撤销的关键是保留抽牌前顶部窗口快照。
    GameOperation operation;
    operation.type = OperationType::DrawCard;
    operation.drawnCard = drawnCard;
    operation.previousOpenTopCards = previousOpenTopCards;
    operation.movedOpenCardToWaste = movedOpenCardToWaste;
    _operations.push_back(operation);
}

// 弹出最近一次操作。
GameOperation OperationHistory::pop()
{
    GameOperation operation = _operations.back();
    _operations.pop_back();
    return operation;
}

// 是否存在可撤销项。
bool OperationHistory::canUndo() const
{
    return !_operations.empty();
}

// 清空操作历史。
void OperationHistory::clear()
{
    _operations.clear();
}
