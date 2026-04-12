#ifndef POKER_GAME_OPERATION_HISTORY_H
#define POKER_GAME_OPERATION_HISTORY_H

#include "PokerCard.h"
#include <vector>

enum class OperationType
{
    Match,
    DrawCard
};

struct GameOperation
{
    OperationType type = OperationType::Match;
    PokerCard movedCard;
    int slotIndex = -1;
    std::vector<int> revealedSlotIndices;
    PokerCard drawnCard;
    std::vector<PokerCard> previousOpenTopCards;
    bool movedOpenCardToWaste = false;
};

class OperationHistory
{
public:
    void recordMatch(const PokerCard& movedCard,
                     int slotIndex,
                     const std::vector<int>& revealedSlotIndices,
                     const std::vector<PokerCard>& previousOpenTopCards);

    void recordDraw(const PokerCard& drawnCard,
                    const std::vector<PokerCard>& previousOpenTopCards,
                    bool movedOpenCardToWaste);

    GameOperation pop();
    bool canUndo() const;
    void clear();

private:
    std::vector<GameOperation> _operations;
};

#endif
