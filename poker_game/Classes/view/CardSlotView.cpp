#include "CardSlotView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

CardSlotView* CardSlotView::create(int slotIndex)
{
    auto* view = new (std::nothrow) CardSlotView();
    if (view != nullptr && view->initWithSlotIndex(slotIndex))
    {
        view->autorelease();
        return view;
    }
    delete view;
    return nullptr;
}

bool CardSlotView::initWithSlotIndex(int slotIndex)
{
    if (!Node::init())
    {
        return false;
    }

    _slotIndex = slotIndex;
    _isDragging = false;
    _touchStartedOnTopCard = false;

    auto& cfg = GlobalConfig::getInstance();

    setContentSize(Size(PokerCardView::getCardWidth(),
                        PokerCardView::getCardHeight() + 200.0f));
    setAnchorPoint(Vec2(0.5f, 0.5f));

    // 触摸事件
    auto* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardSlotView::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CardSlotView::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardSlotView::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void CardSlotView::updateView(const CardSlot& slot)
{
    clearCardViews();

    auto& cfg = GlobalConfig::getInstance();
    float overlapY = cfg.getCardOverlapY();
    const int cardCount = slot.cardCount();

    for (int i = 0; i < cardCount; ++i)
    {
        const bool isTop = (i == cardCount - 1);
        const bool faceUp = isTop ? slot.isTopCardFaceUp() : false;

        auto* cardView = PokerCardView::create(slot.getCard(i), faceUp);
        float offsetY = i * overlapY;
        cardView->setPosition(Vec2(PokerCardView::getCardWidth() * 0.5f,
                                    PokerCardView::getCardHeight() * 0.5f + offsetY));
        addChild(cardView, i);
        _cardViews.push_back(cardView);
    }
}

PokerCardView* CardSlotView::getTopCardView() const
{
    if (_cardViews.empty()) return nullptr;
    return _cardViews.back();
}

cocos2d::Vec2 CardSlotView::getTopCardWorldPosition() const
{
    if (_cardViews.empty())
    {
        return convertToWorldSpace(Vec2(PokerCardView::getCardWidth() * 0.5f,
                                         PokerCardView::getCardHeight() * 0.5f));
    }
    return _cardViews.back()->convertToWorldSpace(Vec2(PokerCardView::getCardWidth() * 0.5f,
                                                        PokerCardView::getCardHeight() * 0.5f));
}

bool CardSlotView::isTopCardFaceUp() const
{
    if (_cardViews.empty()) return false;
    return _cardViews.back()->isFaceUp();
}

int CardSlotView::cardCount() const
{
    return static_cast<int>(_cardViews.size());
}

void CardSlotView::setTapCallback(const TapCallback& cb)
{
    _onDoubleTap = cb;
}

void CardSlotView::setDragStartCallback(const DragStartCallback& cb)
{
    _onDragStart = cb;
}

void CardSlotView::setDragMoveCallback(const DragMoveCallback& cb)
{
    _onDragMove = cb;
}

void CardSlotView::setDragEndCallback(const DragEndCallback& cb)
{
    _onDragEnd = cb;
}

void CardSlotView::setHighlight(bool highlight)
{
    if (!_cardViews.empty())
    {
        _cardViews.back()->setHighlight(highlight);
    }
}

void CardSlotView::animateTopCardBack(cocos2d::CallFunc* callback)
{
    if (_cardViews.empty()) return;
    auto* topCard = _cardViews.back();

    auto& cfg = GlobalConfig::getInstance();

    topCard->setLocalZOrder(static_cast<int>(_topCardOriginalZOrder));

    auto* move = MoveTo::create(cfg.getBounceBackDuration(), _topCardOriginalPos);
    if (callback)
    {
        topCard->runAction(Sequence::create(move, callback, nullptr));
    }
    else
    {
        topCard->runAction(move);
    }
}

void CardSlotView::clearCardViews()
{
    for (auto* view : _cardViews)
    {
        view->removeFromParent();
    }
    _cardViews.clear();
}

bool CardSlotView::hitTestTopCard(const cocos2d::Vec2& worldPos)
{
    if (_cardViews.empty()) return false;
    auto* topCard = _cardViews.back();
    if (!topCard->isFaceUp()) return false;

    Vec2 localPos = topCard->convertToNodeSpace(worldPos);
    Rect cardRect(0, 0, PokerCardView::getCardWidth(), PokerCardView::getCardHeight());
    return cardRect.containsPoint(localPos);
}

bool CardSlotView::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (_cardViews.empty()) return false;

    if (!hitTestTopCard(touch->getLocation())) return false;

    _touchStartedOnTopCard = true;
    _isDragging = false;

    auto* topCard = _cardViews.back();
    _topCardOriginalPos = topCard->getPosition();
    _topCardOriginalZOrder = topCard->getLocalZOrder();

    Vec2 topCardWorldPos = topCard->convertToWorldSpace(
        Vec2(PokerCardView::getCardWidth() * 0.5f, PokerCardView::getCardHeight() * 0.5f));
    _dragOffset = topCardWorldPos - touch->getLocation();

    return true;
}

void CardSlotView::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (!_touchStartedOnTopCard || _cardViews.empty()) return;

    auto& cfg = GlobalConfig::getInstance();

    if (!_isDragging)
    {
        Vec2 startLocal = _cardViews.back()->convertToNodeSpace(touch->getStartLocation());
        Vec2 curLocal = _cardViews.back()->convertToNodeSpace(touch->getLocation());
        float dist = startLocal.distance(curLocal);
        if (dist < cfg.getDragThresholdPx()) return;

        _isDragging = true;

        auto* topCard = _cardViews.back();
        topCard->setLocalZOrder(1000);

        if (_onDragStart)
        {
            Vec2 worldPos = touch->getLocation() + _dragOffset;
            _onDragStart(_slotIndex, worldPos);
        }
    }

    auto* topCard = _cardViews.back();
    if (topCard)
    {
        Vec2 worldPos = touch->getLocation() + _dragOffset;
        Vec2 localPos = convertToNodeSpace(worldPos);
        topCard->setPosition(localPos);

        if (_onDragMove)
        {
            _onDragMove(_slotIndex, worldPos);
        }
    }
}

void CardSlotView::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (!_touchStartedOnTopCard) return;
    _touchStartedOnTopCard = false;

    auto& cfg = GlobalConfig::getInstance();

    if (_isDragging)
    {
        _isDragging = false;

        if (_onDragEnd)
        {
            Vec2 worldPos = touch->getLocation() + _dragOffset;
            _onDragEnd(_slotIndex, worldPos);
        }

        auto* topCard = _cardViews.back();
        if (topCard)
        {
            topCard->setLocalZOrder(static_cast<int>(_topCardOriginalZOrder));
        }
    }
    else
    {
        auto now = std::chrono::steady_clock::now();
        bool isDoubleTap = false;

        if (_slotIndexOfLastTap == _slotIndex)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTapTime).count();
            if (elapsed < cfg.getDoubleClickIntervalMs())
            {
                isDoubleTap = true;
            }
        }

        _lastTapTime = now;
        _slotIndexOfLastTap = _slotIndex;

        if (isDoubleTap)
        {
            if (_onDoubleTap) _onDoubleTap(_slotIndex);
            _slotIndexOfLastTap = -1;
        }
    }
}
