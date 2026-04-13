/**
 * @file GameplayPresenter.cpp
 * @brief 游戏玩法流程协调实现。
 *
 * 主要功能:
 *   - 通过 IGameplayView 接口驱动视图更新
 *   - 管理动画播放、输入锁、胜利检测
 */
#include "presenter/GameplayPresenter.h"

#include "model/GameState.h"
#include "view/PokerCardView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

GameplayPresenter::GameplayPresenter(Node* host,
                                     IGameplayView* view,
                                     GameController& controller,
                                     GameState& state)
    : _host(host)
    , _view(view)
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
    _view->updateHighlights(_controller.getHighlightService().getHighlightStates());
}

void GameplayPresenter::syncTopAreaState(bool animated)
{
    const auto& openTopCards = _controller.getOpenTopCards();
    if (animated)
    {
        _view->animateOpenTopCards(openTopCards);
    }
    else
    {
        _view->setOpenTopCards(openTopCards);
    }

    _view->setReserveCount(_state.reserveDeckSize());
    _view->setWastePileCount(_state.wastePileSize());
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

        auto* topCardView = dynamic_cast<PokerCardView*>(_view->getSlotTopCardView(result.slotIndex));
        if (topCardView == nullptr)
        {
            finishAnimation(cfg.get("matched"));
            break;
        }

        const Vec2 worldPos = topCardView->convertToWorldSpace(Vec2::ZERO);
        const Vec2 parentPos = _host->convertToNodeSpace(worldPos);
        const Vec2 targetParentPos = _host->convertToNodeSpace(_view->getTopCardWorldPosition());

        topCardView->retain();
        topCardView->removeFromParentAndCleanup(false);
        _host->addChild(topCardView, cfg.getFlyingCardZOrder());
        topCardView->setPosition(parentPos);
        topCardView->release();

        const bool won = result.won;
        const int rewardSlot = result.rewardSlotIndex;
        auto* move = MoveTo::create(cfg.getMatchFlyDuration(), targetParentPos);
        auto* callback = CallFunc::create([this, won, rewardSlot]() {
            if (rewardSlot >= 0)
            {
                this->playRewardCardsAnimation(rewardSlot);
            }
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
        _view->updateHighlights(_controller.getHighlightService().getHighlightStates());
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
        const Vec2 topParentPos = _host->convertToNodeSpace(_view->getTopCardWorldPosition());
        cardView->setPosition(topParentPos);
        _host->addChild(cardView, cfg.getFlyingCardZOrder());

        const Vec2 targetWorldPos = _view->getSlotTopCardWorldPosition(result.slotIndex);
        const Vec2 targetParentPos = _host->convertToNodeSpace(
            targetWorldPos == Vec2::ZERO ? _view->getTopCardWorldPosition() : targetWorldPos);

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
        _view->updateHighlights(_controller.getHighlightService().getHighlightStates());
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
    const Rect topCardRect = _view->getTopCardWorldRect();
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
    _view->updateAllSlots(slots);
}

void GameplayPresenter::finishAnimation(const std::string& statusText)
{
    refreshViews();
    setInputLocked(false);
    if (_setStatus) _setStatus(statusText, nullptr);
}

void GameplayPresenter::playRewardCardsAnimation(int rewardSlotIndex)
{
    auto& cfg = GlobalConfig::getInstance();
    const int rewardCount = cfg.getRewardCardsPerBonus();

    const Vec2 rewardWorldPos = _view->getSlotTopCardWorldPosition(rewardSlotIndex);
    if (rewardWorldPos == Vec2::ZERO) return;

    const Vec2 reserveWorldPos = _view->getReserveDeckWorldPosition();
    const Vec2 reserveParentPos = _host->convertToNodeSpace(reserveWorldPos);

    for (int i = 0; i < rewardCount; ++i)
    {
        auto* rewardCard = PokerCardView::create(PokerCard::RewardCard(), false);
        const Vec2 rewardParentPos = _host->convertToNodeSpace(rewardWorldPos);
        rewardCard->setPosition(rewardParentPos);
        rewardCard->setScale(PokerCardView::getCardScale());
        _host->addChild(rewardCard, cfg.getFlyingCardZOrder());

        auto* delay = DelayTime::create(i * cfg.getRewardFlyDelay());
        auto* move = MoveTo::create(cfg.getRewardFlyDuration(), reserveParentPos);
        auto* remove = RemoveSelf::create();
        rewardCard->runAction(Sequence::create(delay, move, remove, nullptr));
    }

    auto* delayAll = DelayTime::create(rewardCount * cfg.getRewardFlyDelay() + cfg.getRewardFlyDuration());
    auto* addCards = CallFunc::create([this, rewardCount]() {
        _view->setReserveCount(_state.reserveDeckSize() + rewardCount);
    });
    _host->runAction(Sequence::create(delayAll, addCards, nullptr));
}
