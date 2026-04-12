#include "history/OperationHistory.h"

void OperationHistory::recordMatch(const PokerCard& movedCard,
                                   int slotIndex,
                                   const std::vector<int>& revealedSlotIndices,
                                   const std::vector<PokerCard>& previousOpenTopCards)
{
    GameOperation operation;
    operation.type = OperationType::Match;
    operation.movedCard = movedCard;
    operation.slotIndex = slotIndex;
    operation.revealedSlotIndices = revealedSlotIndices;
    operation.previousOpenTopCards = previousOpenTopCards;
    _operations.push_back(operation);
}

void OperationHistory::recordDraw(const PokerCard& drawnCard,
                                  const std::vector<PokerCard>& previousOpenTopCards,
                                  bool movedOpenCardToWaste)
{
    GameOperation operation;
    operation.type = OperationType::DrawCard;
    operation.drawnCard = drawnCard;
    operation.previousOpenTopCards = previousOpenTopCards;
    operation.movedOpenCardToWaste = movedOpenCardToWaste;
    _operations.push_back(operation);
}

GameOperation OperationHistory::pop()
{
    GameOperation operation = _operations.back();
    _operations.pop_back();
    return operation;
}

bool OperationHistory::canUndo() const
{
    return !_operations.empty();
}

void OperationHistory::clear()
{
    _operations.clear();
}
