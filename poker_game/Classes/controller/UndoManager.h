#ifndef POKER_GAME_UNDO_MANAGER_H
#define POKER_GAME_UNDO_MANAGER_H

#include "UndoAction.h"
#include <vector>

class UndoManager
{
public:
    UndoManager();

    void pushMatchAction(const PokerCard& movedCard,
                         int fromSlotIndex,
                         const std::vector<int>& revealedSlotIndices,
                         const std::vector<PokerCard>& previousOpenTopCards);

    void pushDrawAction(const PokerCard& drawnCard,
                        const std::vector<PokerCard>& previousOpenTopCards,
                        bool movedOpenCardToWaste);
    UndoAction popLast();
    bool canUndo() const;
    int undoCount() const;
    void clear();

private:
    std::vector<UndoAction> _actionStack;
};

#endif
