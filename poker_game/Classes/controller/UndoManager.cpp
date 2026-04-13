/**
 * @file UndoManager.cpp
 * @brief 撤销管理器实现。
 *
 * 主要功能:
 *   - pushAction: 添加动作到历史
 *   - undo/redo: 撤销或重做最近的操作
 *   - clear: 清空历史记录
 */
#include "UndoManager.h"

UndoManager::UndoManager()
{
}

// 记录一次匹配操作，供撤销使用。
void UndoManager::pushMatchAction(const PokerCard& movedCard,
                                   int fromSlotIndex,
                                   const std::vector<int>& revealedSlotIndices,
                                   const std::vector<PokerCard>& previousOpenTopCards)
{
    // 旧版匹配撤销所需数据与 OperationHistory 基本一致。
    UndoAction action;
    action.type = UndoActionType::Match;
    action.movedCard = movedCard;
    action.fromSlotIndex = fromSlotIndex;
    action.revealedSlotIndices = revealedSlotIndices;
    action.previousOpenTopCards = previousOpenTopCards;
    _actionStack.push_back(action);
}

// 记录一次抽牌操作，包含明牌窗口快照与废牌挤出标记。
void UndoManager::pushDrawAction(const PokerCard& drawnCard,
                                  const std::vector<PokerCard>& previousOpenTopCards,
                                  bool movedOpenCardToWaste)
{
    // 旧版抽牌撤销同样依赖顶部明牌窗口快照。
    UndoAction action;
    action.type = UndoActionType::DrawCard;
    action.drawnCard = drawnCard;
    action.previousOpenTopCards = previousOpenTopCards;
    action.movedOpenCardToWaste = movedOpenCardToWaste;
    _actionStack.push_back(action);
}

// 弹出最近一条记录。
UndoAction UndoManager::popLast()
{
    UndoAction action = _actionStack.back();
    _actionStack.pop_back();
    return action;
}

// 是否存在可撤销记录。
bool UndoManager::canUndo() const
{
    return !_actionStack.empty();
}

// 当前记录数量。
int UndoManager::undoCount() const
{
    return static_cast<int>(_actionStack.size());
}

// 清空撤销栈。
void UndoManager::clear()
{
    _actionStack.clear();
}
