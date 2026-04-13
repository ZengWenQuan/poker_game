/**
 * @file TopAreaView.cpp
 * @brief 顶部区域视图实现。
 *
 * 主要功能:
 *   - 展示明牌窗口 (翻开的主牌区顶牌)
 *   - 展示底牌堆 (背面朝上)
 *   - 回收按钮
 *   - 底牌/废牌数量文案显示
 */
#include "TopAreaView.h"
#include "config/GlobalConfig.h"
#include "UILabelHelper.h"
#include <algorithm>

USING_NS_CC;

// 创建顶部区域视图。
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

// 初始化区域尺寸并构建子控件。
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

// 构建底牌堆节点并绑定触摸。
void TopAreaView::buildReserveDeck()
{
    auto& cfg = GlobalConfig::getInstance();

    Vec2 reservePos = Vec2(_areaWidth * cfg.getReserveStackStartRatio().x,
                           _areaHeight * cfg.getReserveStackStartRatio().y);

    // 底牌堆本体是一个空 Node，具体叠起来的背面牌由 rebuildReserveDeckVisual 动态生成。
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

// 创建底牌/废牌数量文案。
void TopAreaView::buildReserveLabel()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    Vec2 labelPos = Vec2(_areaWidth * cfg.getReserveLabelPositionRatio().x,
                         _areaHeight * cfg.getReserveLabelPositionRatio().y);

    _reserveLabel = UiLabelHelper::create(strings.get("reserve") + "0", theme.getFont(), theme.getFontSize("reserveLabel"));
    _reserveLabel->setPosition(labelPos);
    _reserveLabel->setTextColor(theme.getColor4B("buttonText"));
    addChild(_reserveLabel, 2);
}

// 创建废牌回收按钮并绑定触摸。
void TopAreaView::buildRecycleButton()
{
    auto& cfg = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();
    auto& strings = GlobalConfig::getInstance();

    Vec2 recyclePos = Vec2(_areaWidth * cfg.getRecyclePositionRatio().x,
                           _areaHeight * cfg.getRecyclePositionRatio().y);

    _recycleButton = UiLabelHelper::create(strings.get("recycle"), theme.getFont(), theme.getFontSize("recycle"));
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

// 底牌触摸命中判定。
bool TopAreaView::onReserveTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto& cfg = GlobalConfig::getInstance();
    int padding = cfg.getReserveHitPadding();
    Vec2 localPos = _reserveDeckView->convertToNodeSpace(touch->getLocation());
    // 实际命中区比牌面稍大，避免点边缘时体验太差。
    Rect cardRect(-padding, -padding, PokerCardView::getCardWidth() + padding * 2,
                  PokerCardView::getCardHeight() + padding * 2);
    return cardRect.containsPoint(localPos);
}

// 底牌触摸结束，触发抽牌回调。
void TopAreaView::onReserveTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    CC_UNUSED_PARAM(touch);
    CC_UNUSED_PARAM(event);
    if (_onDraw)
    {
        _onDraw();
    }
}

// 回收按钮触摸命中判定。
bool TopAreaView::onRecycleTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (!_recycleButton->isVisible()) return false;
    auto& cfg = GlobalConfig::getInstance();
    Vec2 localPos = _recycleButton->convertToNodeSpace(touch->getLocation());
    Rect rect = cfg.getRecycleHitRect();
    return rect.containsPoint(localPos);
}

// 回收触摸结束，触发回收回调。
void TopAreaView::onRecycleTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    CC_UNUSED_PARAM(touch);
    CC_UNUSED_PARAM(event);
    if (_onRecycle)
    {
        _onRecycle();
    }
}

// 重建顶部明牌窗口。
void TopAreaView::setOpenTopCards(const std::vector<PokerCard>& cards)
{
    auto& cfg = GlobalConfig::getInstance();

    for (auto* cardView : _openTopCardViews)
    {
        cardView->removeFromParent();
    }
    _openTopCardViews.clear();

    // 顶部明牌从中心向右展开，并带少量叠压，强调最近一张在最右侧。
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

// 获取顶部最右侧明牌的世界坐标（无牌时返回默认位置）。
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

// 获取底牌堆的世界坐标（用于奖励牌飞入动画）。
cocos2d::Vec2 TopAreaView::getReserveDeckWorldPosition() const
{
    return _reserveDeckView->convertToWorldSpace(Vec2::ZERO);
}

// 更新底牌堆数量并刷新显示。
void TopAreaView::setReserveCount(int count)
{
    _reserveCount = count;
    updateReserveVisual();
    updateRecycleButton();
}

// 更新废牌堆数量并刷新回收按钮。
void TopAreaView::setWastePileCount(int count)
{
    _wastePileCount = count;
    updateRecycleButton();
}

// 设置抽牌回调。
void TopAreaView::setDrawCallback(const DrawCallback& cb)
{
    _onDraw = cb;
}

// 设置回收回调。
void TopAreaView::setRecycleCallback(const RecycleCallback& cb)
{
    _onRecycle = cb;
}

// 明牌窗口动画刷新。
void TopAreaView::animateOpenTopCards(const std::vector<PokerCard>& cards)
{
    auto& cfg = GlobalConfig::getInstance();
    setOpenTopCards(cards);
    // 抽牌和撤销抽牌时只做轻量缩放反馈，不引入额外飞行动画。
    for (auto* cardView : _openTopCardViews)
    {
        cardView->setScale(cfg.getTopCardScaleUpFrom());
        cardView->runAction(ScaleTo::create(cfg.getTopCardScaleUpDuration(), 1.0f));
    }
}

// 获取顶部牌区域的世界包围盒，供拖拽命中使用。
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

// 刷新底牌/废牌文案与底牌堆可见性。
void TopAreaView::updateReserveVisual()
{
    auto& strings = GlobalConfig::getInstance();
    // reserve / waste 合并显示在一条文案里，减少顶部区域元素数量。
    std::string wasteInfo = _wastePileCount > 0 ? (strings.get("waste") + std::to_string(_wastePileCount)) : "";
    _reserveLabel->setString(strings.get("reserve") + std::to_string(_reserveCount) + wasteInfo);
    _reserveDeckView->setVisible(_reserveCount > 0);

    rebuildReserveDeckVisual();
}

// 控制回收按钮显隐：仅当底牌耗尽且有废牌时显示。
void TopAreaView::updateRecycleButton()
{
    _recycleButton->setVisible(_reserveCount == 0 && _wastePileCount > 0);
}

// 重新生成叠放的底牌背面视觉。
void TopAreaView::rebuildReserveDeckVisual()
{
    _reserveDeckView->removeAllChildren();
    if (_reserveCount <= 0) return;

    auto& cfg = GlobalConfig::getInstance();
    const float stepX = PokerCardView::getCardWidth() * cfg.getReserveStackStepXRatio();

    // 用多张背面牌叠出“还剩多少底牌”的视觉暗示。
    for (int i = 0; i < _reserveCount; ++i)
    {
        auto* cardBack = PokerCardView::create(PokerCard(CardSuit::Spade, CardRank::Ace), false);
        cardBack->setPosition(Vec2(-stepX * static_cast<float>(i), 0.0f));
        _reserveDeckView->addChild(cardBack, _reserveCount - i);
    }
}

void TopAreaView::refreshStrings()
{
    auto& strings = GlobalConfig::getInstance();
    auto& theme = GlobalConfig::getInstance();

    // 底牌/废牌数量文案
    std::string wasteInfo = _wastePileCount > 0 ? (strings.get("waste") + std::to_string(_wastePileCount)) : "";
    _reserveLabel->setString(strings.get("reserve") + std::to_string(_reserveCount) + wasteInfo);

    // 回收按钮
    _recycleButton->setString(strings.get("recycle"));
}
