#ifndef POKER_GAME_UNDO_ACTION_H
#define POKER_GAME_UNDO_ACTION_H

#include "PokerCard.h"
#include <vector>

enum class UndoActionType
{
    Match,
    DrawCard
};

struct UndoAction
{
    UndoActionType type;

    // Match 操作
    PokerCard movedCard;
    int fromSlotIndex = -1;
    std::vector<int> revealedSlotIndices;

    // DrawCard 操作
    PokerCard drawnCard;

    std::vector<PokerCard> previousOpenTopCards;
    bool movedOpenCardToWaste = false;
};

#endif
