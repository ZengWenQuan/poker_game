#include "GameController.h"

GameController::GameController(GameState& state)
    : _state(state)
{
}

void GameController::startGame()
{
    _state.initializeGame();
    _operationHistory.clear();
}

GameActionResult GameController::onSlotCardTapped(int slotIndex)
{
    GameActionResult result;
    if (slotIndex < 0 || slotIndex >= _state.slotCount()) return result;

    const auto& slot = _state.getSlot(slotIndex);
    if (slot.isEmpty() || !slot.isTopCardFaceUp()) return result;

    const PokerCard& slotCard = slot.topCard();
    const auto& openTopCards = _state.getOpenTopCards();
    int matchedTopIndex = -1;
    for (int i = 0; i < static_cast<int>(openTopCards.size()); ++i)
    {
        if (MatchEngine::canMatch(slotCard, openTopCards[i]))
        {
            matchedTopIndex = i;
            break;
        }
    }

    if (matchedTopIndex < 0)
    {
        result.type = GameActionType::MatchFail;
        return result;
    }

    std::vector<PokerCard> previousOpenTopCards = _state.getOpenTopCards();
    std::vector<int> revealedSlotIndices;
    PokerCard movedCard = _state.removeCardFromSlot(slotIndex, &revealedSlotIndices);

    _operationHistory.recordMatch(movedCard, slotIndex, revealedSlotIndices, previousOpenTopCards);

    _state.replaceOpenTopCard(matchedTopIndex, movedCard);

    result.type = GameActionType::MatchSuccess;
    result.slotIndex = slotIndex;
    result.card = movedCard;
    result.won = _state.isWin();
    return result;
}

GameActionResult GameController::onDrawReserveCard()
{
    GameActionResult result;
    if (!_state.hasReserveCards()) return result;

    std::vector<PokerCard> previousOpenTopCards = _state.getOpenTopCards();
    const bool movedOpenCardToWaste =
        static_cast<int>(previousOpenTopCards.size()) >= _state.getVisibleTopCardCount();

    _state.drawFromReserve();
    PokerCard drawnCard = _state.getCurrentTopCard();

    _operationHistory.recordDraw(drawnCard, previousOpenTopCards, movedOpenCardToWaste);
    result.type = GameActionType::Draw;
    result.card = drawnCard;
    return result;
}

GameActionResult GameController::onRecycleWastePile()
{
    GameActionResult result;
    if (_state.hasReserveCards() || _state.wastePileSize() == 0) return result;

    _state.recycleWastePile();

    _operationHistory.clear();
    result.type = GameActionType::Recycle;
    return result;
}

GameActionResult GameController::onUndo()
{
    GameActionResult result;
    if (!_operationHistory.canUndo())
    {
        result.type = GameActionType::NoUndo;
        return result;
    }

    GameOperation operation = _operationHistory.pop();

    switch (operation.type)
    {
    case OperationType::Match:
    {
        _state.restoreOpenTopCards(operation.previousOpenTopCards);
        _state.restoreCardToSlot(operation.slotIndex, operation.movedCard, operation.revealedSlotIndices);
        result.type = GameActionType::UndoMatch;
        result.slotIndex = operation.slotIndex;
        result.card = operation.movedCard;
        break;
    }
    case OperationType::DrawCard:
    {
        _state.returnCardToReserve(operation.drawnCard);
        _state.restoreOpenTopCards(operation.previousOpenTopCards);
        if (operation.movedOpenCardToWaste)
        {
            _state.popFromWastePile();
        }
        result.type = GameActionType::UndoDraw;
        break;
    }
    }

    return result;
}

const std::vector<PokerCard>& GameController::getOpenTopCards() const { return _state.getOpenTopCards(); }

bool GameController::isSlotMatchable(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= _state.slotCount()) return false;
    const auto& slot = _state.getSlot(slotIndex);
    if (slot.isEmpty() || !slot.isTopCardFaceUp()) return false;
    for (const auto& topCard : _state.getOpenTopCards())
    {
        if (MatchEngine::canMatch(slot.topCard(), topCard))
        {
            return true;
        }
    }
    return false;
}

std::vector<bool> GameController::getHighlightStates() const
{
    std::vector<bool> highlightStates;
    highlightStates.reserve(_state.slotCount());

    for (int i = 0; i < _state.slotCount(); ++i)
    {
        highlightStates.push_back(isSlotMatchable(i));
    }

    return highlightStates;
}
