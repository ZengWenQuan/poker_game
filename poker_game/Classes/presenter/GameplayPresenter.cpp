#include "presenter/GameplayPresenter.h"

#include "model/GameState.h"
#include "view/MainAreaView.h"
#include "view/PokerCardView.h"
#include "view/TopAreaView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

GameplayPresenter::GameplayPresenter(Node* host,
                                     MainAreaView* mainArea,
                                     TopAreaView* topArea,
                                     GameController& controller,
                                     GameState& state)
    : _host(host)
    , _mainArea(mainArea)
    , _topArea(topArea)
    , _controller(controller)
    , _state(state)
{
}

void GameplayPresenter::setStatusCallback(const StatusCallback& cb)
{
    _setStatus = cb;
}

void GameplayPresenter::refreshViews()
{
    refreshMainArea();
    syncTopAreaState(false);
    _mainArea->updateHighlights(_controller.getHighlightStates());
}

void GameplayPresenter::syncTopAreaState(bool animated)
{
    const auto& openTopCards = _controller.getOpenTopCards();
    if (animated)
    {
        _topArea->animateOpenTopCards(openTopCards);
    }
    else
    {
        _topArea->setOpenTopCards(openTopCards);
    }

    _topArea->setReserveCount(_state.reserveDeckSize());
    _topArea->setWastePileCount(_state.wastePileSize());
}

void GameplayPresenter::handleResult(const GameActionResult& result)
{
    auto& cfg = GlobalConfig::getInstance();

    switch (result.type)
    {
    case GameActionType::None:
        return;
    case GameActionType::MatchSuccess:
    {
        setInputLocked(true);

        auto* slotView = _mainArea->getSlotView(result.slotIndex);
        auto* topCardView = slotView ? slotView->getTopCardView() : nullptr;
        if (topCardView == nullptr)
        {
            finishAnimation(cfg.get("matched"));
            break;
        }

        const Vec2 worldPos = topCardView->convertToWorldSpace(Vec2::ZERO);
        const Vec2 parentPos = _host->convertToNodeSpace(worldPos);
        const Vec2 targetParentPos = _host->convertToNodeSpace(_topArea->getTopCardWorldPosition());

        topCardView->retain();
        topCardView->removeFromParentAndCleanup(false);
        _host->addChild(topCardView, 10);
        topCardView->setPosition(parentPos);
        topCardView->release();

        const bool won = result.won;
        auto* move = MoveTo::create(cfg.getMatchFlyDuration(), targetParentPos);
        auto* callback = CallFunc::create([this, won]() {
            finishAnimation(GlobalConfig::getInstance().get("matched"));
            if (won && _setStatus)
            {
                const Color4B winColor = GlobalConfig::getInstance().getColor4B("winGold");
                _setStatus(GlobalConfig::getInstance().get("youWin"), &winColor);
            }
        });
        topCardView->runAction(Sequence::create(move, callback, RemoveSelf::create(), nullptr));
        break;
    }
    case GameActionType::MatchFail:
        if (_setStatus) _setStatus(cfg.get("cannotMatch"), nullptr);
        break;
    case GameActionType::Draw:
        setInputLocked(true);
        syncTopAreaState(true);
        _mainArea->updateHighlights(_controller.getHighlightStates());
        if (_setStatus) _setStatus(cfg.get("drew"), nullptr);
        _host->runAction(Sequence::create(DelayTime::create(cfg.getDrawDelayDuration()),
                                          CallFunc::create([this]() { setInputLocked(false); }),
                                          nullptr));
        break;
    case GameActionType::Recycle:
        refreshViews();
        if (_setStatus) _setStatus(cfg.get("recycled"), nullptr);
        break;
    case GameActionType::UndoMatch:
    {
        setInputLocked(true);
        syncTopAreaState(false);

        auto* cardView = PokerCardView::create(result.card, true);
        const Vec2 topParentPos = _host->convertToNodeSpace(_topArea->getTopCardWorldPosition());
        cardView->setPosition(topParentPos);
        _host->addChild(cardView, 10);

        auto* slotView = _mainArea->getSlotView(result.slotIndex);
        const Vec2 targetWorldPos = slotView ? slotView->getTopCardWorldPosition() : _topArea->getTopCardWorldPosition();
        const Vec2 targetParentPos = _host->convertToNodeSpace(targetWorldPos);

        auto* move = MoveTo::create(cfg.getUndoFlyDuration(), targetParentPos);
        auto* callback = CallFunc::create([this]() {
            finishAnimation(GlobalConfig::getInstance().get("undoMatch"));
        });
        cardView->runAction(Sequence::create(move, callback, RemoveSelf::create(), nullptr));
        break;
    }
    case GameActionType::UndoDraw:
        setInputLocked(true);
        syncTopAreaState(true);
        _mainArea->updateHighlights(_controller.getHighlightStates());
        setInputLocked(false);
        if (_setStatus) _setStatus(cfg.get("undoDraw"), nullptr);
        break;
    case GameActionType::NoUndo:
        if (_setStatus) _setStatus(cfg.get("nothingToUndo"), nullptr);
        break;
    }
}

void GameplayPresenter::setInputLocked(bool locked)
{
    _inputLocked = locked;
}

bool GameplayPresenter::isInputLocked() const
{
    return _inputLocked;
}

bool GameplayPresenter::isNearTopCardArea(const Vec2& worldPos) const
{
    auto& cfg = GlobalConfig::getInstance();
    const Rect topCardRect = _topArea->getTopCardWorldRect();
    const float padding = PokerCardView::getCardWidth() * cfg.getDropAreaPaddingRatio();
    return Rect(topCardRect.origin.x - padding,
                topCardRect.origin.y - padding,
                topCardRect.size.width + padding * 2,
                topCardRect.size.height + padding * 2).containsPoint(worldPos);
}

void GameplayPresenter::refreshMainArea()
{
    std::vector<CardSlot> slots;
    for (int i = 0; i < _state.slotCount(); ++i)
    {
        slots.push_back(_state.getSlot(i));
    }
    _mainArea->updateAllSlots(slots);
}

void GameplayPresenter::finishAnimation(const std::string& statusText)
{
    refreshViews();
    setInputLocked(false);
    if (_setStatus) _setStatus(statusText, nullptr);
}
