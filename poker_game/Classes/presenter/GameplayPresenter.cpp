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

// 设置状态栏输出回调。
void GameplayPresenter::setStatusCallback(const StatusCallback& cb)
{
    _setStatus = cb;
}

// 全量刷新主牌区、顶部牌区和高亮。
void GameplayPresenter::refreshViews()
{
    // 主牌区、顶部牌区和高亮都依赖当前最新状态，统一在这里刷新。
    refreshMainArea();
    syncTopAreaState(false);
    _mainArea->updateHighlights(_controller.getHighlightStates());
}

// 同步顶部区域状态，可选动画。
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

    // 顶部明牌窗口与 reserve / waste 计数必须同步更新。
    _topArea->setReserveCount(_state.reserveDeckSize());
    _topArea->setWastePileCount(_state.wastePileSize());
}

// 根据一次游戏动作结果驱动界面刷新和动画。
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

        // 为了做“从主牌区飞向顶部”的动画，需要把卡牌节点临时提到场景根节点。
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
        // 抽牌只需要短暂锁输入，等待顶部缩放动画完成即可。
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

// 当前是否因动画等原因锁定输入。
bool GameplayPresenter::isInputLocked() const
{
    return _inputLocked;
}

// 判断某世界坐标是否靠近顶部牌区，供拖拽落点判定。
bool GameplayPresenter::isNearTopCardArea(const Vec2& worldPos) const
{
    auto& cfg = GlobalConfig::getInstance();
    const Rect topCardRect = _topArea->getTopCardWorldRect();
    const float padding = PokerCardView::getCardWidth() * cfg.getDropAreaPaddingRatio();
    // 拖放判定不要求严格落在牌面内部，额外扩大一圈容错区域。
    return Rect(topCardRect.origin.x - padding,
                topCardRect.origin.y - padding,
                topCardRect.size.width + padding * 2,
                topCardRect.size.height + padding * 2).containsPoint(worldPos);
}

// 把状态层槽位复制给视图批量刷新。
void GameplayPresenter::refreshMainArea()
{
    std::vector<CardSlot> slots;
    // MainAreaView 目前以批量刷新为主，这里把状态层槽位拷出来交给 View。
    for (int i = 0; i < _state.slotCount(); ++i)
    {
        slots.push_back(_state.getSlot(i));
    }
    _mainArea->updateAllSlots(slots);
}

// 动画收尾：解锁输入、刷新视图并更新状态文本。
void GameplayPresenter::finishAnimation(const std::string& statusText)
{
    refreshViews();
    setInputLocked(false);
    if (_setStatus) _setStatus(statusText, nullptr);
}
