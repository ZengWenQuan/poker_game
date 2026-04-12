#ifndef POKER_GAME_GAME_CONTROLLER_H
#define POKER_GAME_GAME_CONTROLLER_H

#include "GameState.h"
#include "MatchEngine.h"
#include "history/OperationHistory.h"

enum class GameActionType
{
    None,
    MatchSuccess,
    MatchFail,
    Draw,
    Recycle,
    UndoMatch,
    UndoDraw,
    NoUndo
};

struct GameActionResult
{
    GameActionType type = GameActionType::None;
    int slotIndex = -1;
    PokerCard card;
    bool won = false;
};

class GameController
{
public:
    explicit GameController(GameState& state);

    void startGame();

    GameActionResult onSlotCardTapped(int slotIndex);
    GameActionResult onDrawReserveCard();
    GameActionResult onRecycleWastePile();
    GameActionResult onUndo();

    const std::vector<PokerCard>& getOpenTopCards() const;
    bool isSlotMatchable(int slotIndex) const;
    std::vector<bool> getHighlightStates() const;

private:
    GameState& _state;
    OperationHistory _operationHistory;
};

#endif
