#include "UndoManager.h"

UndoManager::UndoManager()
{
}

void UndoManager::pushMatchAction(const PokerCard& movedCard,
                                   int fromSlotIndex,
                                   const std::vector<int>& revealedSlotIndices,
                                   const std::vector<PokerCard>& previousOpenTopCards)
{
    UndoAction action;
    action.type = UndoActionType::Match;
    action.movedCard = movedCard;
    action.fromSlotIndex = fromSlotIndex;
    action.revealedSlotIndices = revealedSlotIndices;
    action.previousOpenTopCards = previousOpenTopCards;
    _actionStack.push_back(action);
}

void UndoManager::pushDrawAction(const PokerCard& drawnCard,
                                  const std::vector<PokerCard>& previousOpenTopCards,
                                  bool movedOpenCardToWaste)
{
    UndoAction action;
    action.type = UndoActionType::DrawCard;
    action.drawnCard = drawnCard;
    action.previousOpenTopCards = previousOpenTopCards;
    action.movedOpenCardToWaste = movedOpenCardToWaste;
    _actionStack.push_back(action);
}

UndoAction UndoManager::popLast()
{
    UndoAction action = _actionStack.back();
    _actionStack.pop_back();
    return action;
}

bool UndoManager::canUndo() const
{
    return !_actionStack.empty();
}

int UndoManager::undoCount() const
{
    return static_cast<int>(_actionStack.size());
}

void UndoManager::clear()
{
    _actionStack.clear();
}
