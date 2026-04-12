#ifndef POKER_GAME_UNDO_ACTION_H
#define POKER_GAME_UNDO_ACTION_H

#include "PokerCard.h"
#include <vector>

enum class UndoActionType
{
    Match,    // 旧版匹配撤销记录
    DrawCard  // 旧版抽牌撤销记录
};

struct UndoAction
{
    UndoActionType type; // 撤销动作类型

    // Match 操作
    PokerCard movedCard;
    int fromSlotIndex = -1;
    std::vector<int> revealedSlotIndices;

    // DrawCard 操作
    PokerCard drawnCard;

    std::vector<PokerCard> previousOpenTopCards; // 操作前顶部明牌窗口
    bool movedOpenCardToWaste = false;           // 抽牌时是否挤出了旧明牌
};

#endif
