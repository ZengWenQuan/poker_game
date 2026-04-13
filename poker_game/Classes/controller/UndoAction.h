/**
 * @file UndoAction.h
 * @brief 撤销动作数据结构。
 *
 * 主要功能:
 *   - MatchAction : 记录匹配操作 (移出的卡槽索引)
 *   - FlipAction  : 记录翻牌操作 (翻开/合上)
 *   - RecycleAction: 记录回收操作
 */
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
