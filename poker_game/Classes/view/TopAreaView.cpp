#include "TopAreaView.h"
#include "config/GlobalConfig.h"
#include <algorithm>

USING_NS_CC;

TopAreaView* TopAreaView::create()
{
    auto* view = new (std::nothrow) TopAreaView();
    if (view != nullptr && view->init())
    {
        view->autorelease();
        return view;
    }
    delete view;
    return nullptr;
}

bool TopAreaView::init()
{
    if (!Node::init())
    {
        return false;
    }

    auto& cfg = GlobalConfig::getInstance();

    _areaWidth = cfg.getDesignWidth();
    _areaHeight = cfg.getDesignHeight();

    setContentSize(Size(_areaWidth, _areaHeight));
    setAnchorPoint(Vec2(0.5f, 0.5f));

    buildReserveDeck();
    buildReserveLabel();
    buildRecycleButton();

    return true;
}

void TopAreaView::buildReserveDeck()
{
    auto& cfg = GlobalConfig::getInstance();

    Vec2 reservePos = Vec2(_areaWidth * cfg.getReserveStackStartRatio().x,
                           _areaHeight * cfg.getReserveStackStartRatio().y);

    _reserveDeckView = Node::create();
    _reserveDeckView->setPosition(reservePos);
    addChild(_reserveDeckView, 1);

    auto* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TopAreaView::onReserveTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(TopAreaView::onReserveTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _reserveDeckView);

    rebuildReserveDeckVisual();
}

void TopAreaView::buildReserveLabel()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    Vec2 labelPos = Vec2(_areaWidth * cfg.getReserveLabelPositionRatio().x,
                         _areaHeight * cfg.getReserveLabelPositionRatio().y);

    _reserveLabel = Label::createWithSystemFont(strings.get("reserve") + "0", theme.getFont(), theme.getFontSize("reserveLabel"));
    _reserveLabel->setPosition(labelPos);
    _reserveLabel->setTextColor(Color4B::WHITE);
    addChild(_reserveLabel, 2);
}

void TopAreaView::buildRecycleButton()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    Vec2 recyclePos = Vec2(_areaWidth * cfg.getRecyclePositionRatio().x,
                           _areaHeight * cfg.getRecyclePositionRatio().y);

    _recycleButton = Label::createWithSystemFont(strings.get("recycle"), theme.getFont(), theme.getFontSize("recycle"));
    _recycleButton->setTextColor(theme.getColor4B("goldHighlight"));
    _recycleButton->setPosition(recyclePos);
    _recycleButton->setVisible(false);
    addChild(_recycleButton, 3);

    auto* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(TopAreaView::onRecycleTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(TopAreaView::onRecycleTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _recycleButton);
}

bool TopAreaView::onReserveTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto& cfg = GlobalConfig::getInstance();
    int padding = cfg.getReserveHitPadding();
    Vec2 localPos = _reserveDeckView->convertToNodeSpace(touch->getLocation());
    Rect cardRect(-padding, -padding, PokerCardView::getCardWidth() + padding * 2,
                  PokerCardView::getCardHeight() + padding * 2);
    return cardRect.containsPoint(localPos);
}

void TopAreaView::onReserveTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    CC_UNUSED_PARAM(touch);
    CC_UNUSED_PARAM(event);
    if (_onDraw)
    {
        _onDraw();
    }
}

bool TopAreaView::onRecycleTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (!_recycleButton->isVisible()) return false;
    auto& cfg = GlobalConfig::getInstance();
    Vec2 localPos = _recycleButton->convertToNodeSpace(touch->getLocation());
    Rect rect = cfg.getRecycleHitRect();
    return rect.containsPoint(localPos);
}

void TopAreaView::onRecycleTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    CC_UNUSED_PARAM(touch);
    CC_UNUSED_PARAM(event);
    if (_onRecycle)
    {
        _onRecycle();
    }
}

void TopAreaView::setOpenTopCards(const std::vector<PokerCard>& cards)
{
    auto& cfg = GlobalConfig::getInstance();

    for (auto* cardView : _openTopCardViews)
    {
        cardView->removeFromParent();
    }
    _openTopCardViews.clear();

    const Vec2 centerPos = Vec2(_areaWidth * cfg.getTopCardPositionRatio().x,
                                _areaHeight * cfg.getTopCardPositionRatio().y);
    const float spacingX = PokerCardView::getCardWidth() * 1.5f;
    const float startX = centerPos.x - spacingX * 0.5f * static_cast<float>(std::max(0, static_cast<int>(cards.size()) - 1));

    for (int i = 0; i < static_cast<int>(cards.size()); ++i)
    {
        auto* cardView = PokerCardView::create(cards[i], true);
        const float overlapOffsetX = PokerCardView::getCardWidth() * 0.4f * static_cast<float>(i);
        cardView->setPosition(Vec2(startX + spacingX * static_cast<float>(i) + overlapOffsetX,
                                   centerPos.y));
        addChild(cardView, 2 + i);
        _openTopCardViews.push_back(cardView);
    }
}

cocos2d::Vec2 TopAreaView::getTopCardWorldPosition() const
{
    auto& cfg = GlobalConfig::getInstance();
    Vec2 defaultPos = Vec2(_areaWidth * cfg.getTopCardPositionRatio().x,
                           _areaHeight * cfg.getTopCardPositionRatio().y);

    if (!_openTopCardViews.empty())
    {
        return _openTopCardViews.back()->convertToWorldSpace(Vec2::ZERO);
    }
    return convertToWorldSpace(defaultPos);
}

void TopAreaView::setReserveCount(int count)
{
    _reserveCount = count;
    updateReserveVisual();
    updateRecycleButton();
}

void TopAreaView::setWastePileCount(int count)
{
    _wastePileCount = count;
    updateRecycleButton();
}

void TopAreaView::setDrawCallback(const DrawCallback& cb)
{
    _onDraw = cb;
}

void TopAreaView::setRecycleCallback(const RecycleCallback& cb)
{
    _onRecycle = cb;
}

void TopAreaView::animateOpenTopCards(const std::vector<PokerCard>& cards)
{
    auto& cfg = GlobalConfig::getInstance();
    setOpenTopCards(cards);
    for (auto* cardView : _openTopCardViews)
    {
        cardView->setScale(0.8f);
        cardView->runAction(ScaleTo::create(cfg.getTopCardScaleUpDuration(), 1.0f));
    }
}

cocos2d::Rect TopAreaView::getTopCardWorldRect() const
{
    if (_openTopCardViews.empty())
    {
        Vec2 worldPos = getTopCardWorldPosition();
        float w = PokerCardView::getCardWidth();
        float h = PokerCardView::getCardHeight();
        return Rect(worldPos.x, worldPos.y, w, h);
    }

    Rect unionRect;
    bool initialized = false;
    for (auto* cardView : _openTopCardViews)
    {
        Vec2 worldPos = cardView->convertToWorldSpace(Vec2::ZERO);
        Rect rect(worldPos.x, worldPos.y, PokerCardView::getCardWidth(), PokerCardView::getCardHeight());
        if (!initialized)
        {
            unionRect = rect;
            initialized = true;
        }
        else
        {
            unionRect.merge(rect);
        }
    }
    return unionRect;
}

void TopAreaView::updateReserveVisual()
{
    auto& strings = GlobalConfig::getInstance();
    std::string wasteInfo = _wastePileCount > 0 ? (strings.get("waste") + std::to_string(_wastePileCount)) : "";
    _reserveLabel->setString(strings.get("reserve") + std::to_string(_reserveCount) + wasteInfo);
    _reserveDeckView->setVisible(_reserveCount > 0);

    rebuildReserveDeckVisual();
}

void TopAreaView::updateRecycleButton()
{
    _recycleButton->setVisible(_reserveCount == 0 && _wastePileCount > 0);
}

void TopAreaView::rebuildReserveDeckVisual()
{
    _reserveDeckView->removeAllChildren();
    if (_reserveCount <= 0) return;

    auto& cfg = GlobalConfig::getInstance();
    const float stepX = PokerCardView::getCardWidth() * cfg.getReserveStackStepXRatio();

    for (int i = 0; i < _reserveCount; ++i)
    {
        auto* cardBack = PokerCardView::create(PokerCard(CardSuit::Spade, CardRank::Ace), false);
        cardBack->setPosition(Vec2(-stepX * static_cast<float>(i), 0.0f));
        _reserveDeckView->addChild(cardBack, _reserveCount - i);
    }
}
