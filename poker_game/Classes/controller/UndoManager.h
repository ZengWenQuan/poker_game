#ifndef POKER_GAME_UNDO_MANAGER_H
#define POKER_GAME_UNDO_MANAGER_H

#include "UndoAction.h"
#include <vector>

// 撤销管理器：以栈形式记录玩家操作，提供回溯能力。
class UndoManager
{
public:
    UndoManager();

    // 旧版撤销栈实现，目前保留主要是兼容历史代码。
    // movedCard: 本次移动的牌；fromSlotIndex: 移动前所在槽位；
    // revealedSlotIndices: 移动后被翻开的槽位索引；previousOpenTopCards: 操作前的顶部明牌窗口。
    void pushMatchAction(const PokerCard& movedCard,
                         int fromSlotIndex,
                         const std::vector<int>& revealedSlotIndices,
                         const std::vector<PokerCard>& previousOpenTopCards);

    // 记录一次抽牌动作。
    // drawnCard: 本次抽出的牌；previousOpenTopCards: 抽牌前的顶部明牌；
    // movedOpenCardToWaste: 抽牌时是否把旧明牌挤入废牌堆。
    void pushDrawAction(const PokerCard& drawnCard,
                        const std::vector<PokerCard>& previousOpenTopCards,
                        bool movedOpenCardToWaste);
    // 弹出最近一条操作记录。
    UndoAction popLast();
    // 是否存在可撤销动作。
    bool canUndo() const;
    // 当前撤销栈元素数量。
    int undoCount() const;
    // 清空撤销栈。
    void clear();

private:
    std::vector<UndoAction> _actionStack; // 最近操作位于尾部
};

#endif
