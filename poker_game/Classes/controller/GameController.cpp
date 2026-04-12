#include "GameController.h"

GameController::GameController(GameState& state)
    : _state(state)
{
}

// 开新局：清空状态与撤销记录。
void GameController::startGame()
{
    // 新局开始时，状态和撤销历史必须同步清空。
    _state.initializeGame();
    _operationHistory.clear();
}

// 处理主牌区点击，尝试与顶部明牌匹配。
// slotIndex: 被点击的槽位索引。
GameActionResult GameController::onSlotCardTapped(int slotIndex)
{
    GameActionResult result;
    if (slotIndex < 0 || slotIndex >= _state.slotCount()) return result;

    const auto& slot = _state.getSlot(slotIndex);
    // 只有当前可见的顶牌才允许被点击参与匹配。
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

    // 先记录顶部窗口旧状态，再执行状态变更，保证撤销可以回到完整前态。
    std::vector<PokerCard> previousOpenTopCards = _state.getOpenTopCards();
    std::vector<int> revealedSlotIndices;
    PokerCard movedCard = _state.removeCardFromSlot(slotIndex, &revealedSlotIndices);

    _operationHistory.recordMatch(movedCard, slotIndex, revealedSlotIndices, previousOpenTopCards);

    // 匹配成功后，用主牌区移出的牌替换掉被匹配的顶部明牌。
    _state.replaceOpenTopCard(matchedTopIndex, movedCard);

    result.type = GameActionType::MatchSuccess;
    result.slotIndex = slotIndex;
    result.card = movedCard;
    result.won = _state.isWin();
    return result;
}

// 抽一张底牌到顶部窗口，记录撤销所需状态。
GameActionResult GameController::onDrawReserveCard()
{
    GameActionResult result;
    if (!_state.hasReserveCards()) return result;

    std::vector<PokerCard> previousOpenTopCards = _state.getOpenTopCards();
    // 若顶部窗口已满，抽牌会把最旧明牌挤入废牌堆，撤销时要回滚这一步。
    const bool movedOpenCardToWaste =
        static_cast<int>(previousOpenTopCards.size()) >= _state.getVisibleTopCardCount();

    _state.drawFromReserve();
    PokerCard drawnCard = _state.getCurrentTopCard();

    _operationHistory.recordDraw(drawnCard, previousOpenTopCards, movedOpenCardToWaste);
    result.type = GameActionType::Draw;
    result.card = drawnCard;
    return result;
}

// 回收废牌堆到底牌堆；回收后清空撤销历史。
GameActionResult GameController::onRecycleWastePile()
{
    GameActionResult result;
    if (_state.hasReserveCards() || _state.wastePileSize() == 0) return result;

    _state.recycleWastePile();

    // 回收会整体改变抽牌链路，旧撤销历史已经失真，直接清空最稳妥。
    _operationHistory.clear();
    result.type = GameActionType::Recycle;
    return result;
}

// 撤销最近一次匹配或抽牌。
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
        // 撤销匹配时，先恢复顶部窗口，再恢复主牌区与翻牌状态。
        _state.restoreOpenTopCards(operation.previousOpenTopCards);
        _state.restoreCardToSlot(operation.slotIndex, operation.movedCard, operation.revealedSlotIndices);
        result.type = GameActionType::UndoMatch;
        result.slotIndex = operation.slotIndex;
        result.card = operation.movedCard;
        break;
    }
    case OperationType::DrawCard:
    {
        // 撤销抽牌时，抽出的牌回到底牌堆顶部，再回滚明牌窗口和废牌堆。
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

// 槽位是否可与任意顶部明牌匹配，用于高亮判定。
bool GameController::isSlotMatchable(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= _state.slotCount()) return false;
    const auto& slot = _state.getSlot(slotIndex);
    if (slot.isEmpty() || !slot.isTopCardFaceUp()) return false;

    // 只要能和任意顶部明牌匹配，就认为这个槽位应被高亮。
    for (const auto& topCard : _state.getOpenTopCards())
    {
        if (MatchEngine::canMatch(slot.topCard(), topCard))
        {
            return true;
        }
    }
    return false;
}

// 返回全部槽位的可匹配状态数组。
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
