#include "MainAreaView.h"
#include "PokerCardView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

MainAreaView* MainAreaView::create()
{
    auto* view = new (std::nothrow) MainAreaView();
    if (view != nullptr && view->init())
    {
        view->autorelease();
        return view;
    }
    delete view;
    return nullptr;
}

bool MainAreaView::init()
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
    return true;
}

void MainAreaView::setupFromLayout(const LayoutConfig& layout)
{
    for (auto* view : _slotViews)
    {
        view->removeFromParent();
    }
    _slotViews.clear();

    const auto& slots = layout.getSlots();
    int maxLayer = 0;
    for (const auto& slotLayout : slots)
    {
        maxLayer = std::max(maxLayer, slotLayout.layer);
    }

    for (const auto& slotLayout : slots)
    {
        int index = slotLayout.id;
        auto* slotView = CardSlotView::create(index);
        const Vec2 resolvedPos = LayoutConfig::resolveSlotPosition(slotLayout,
                                                                   layout.usePileCenterOrigin(),
                                                                   layout.getMainPileCount(),
                                                                   _areaWidth,
                                                                   _areaHeight);

        // 双击回调
        slotView->setTapCallback([this](int slotIndex) {
            if (_onSlotDoubleTap) _onSlotDoubleTap(slotIndex);
        });

        // 拖动回调
        slotView->setDragStartCallback([this](int slotIndex, const Vec2& worldPos) {
            if (_onDragStart) _onDragStart(slotIndex, worldPos);
        });
        slotView->setDragMoveCallback([this](int slotIndex, const Vec2& worldPos) {
            if (_onDragMove) _onDragMove(slotIndex, worldPos);
        });
        slotView->setDragEndCallback([this](int slotIndex, const Vec2& worldPos) {
            if (_onDragEnd) _onDragEnd(slotIndex, worldPos);
        });

        // 旋转：以左下角为圆心，顺时针为正
        if (slotLayout.rotation != 0.0f)
        {
            slotView->setAnchorPoint(Vec2(0.0f, 0.0f));
            slotView->setPosition(Vec2(resolvedPos.x - PokerCardView::getCardWidth() * 0.5f,
                                        resolvedPos.y - PokerCardView::getCardHeight() * 0.5f));
            slotView->setRotation(slotLayout.rotation);
        }
        else
        {
            slotView->setPosition(resolvedPos);
        }

        addChild(slotView, maxLayer - slotLayout.layer);

        _slotViews.push_back(slotView);
    }
}

void MainAreaView::updateAllSlots(const std::vector<CardSlot>& slots)
{
    for (int i = 0; i < static_cast<int>(slots.size()) && i < static_cast<int>(_slotViews.size()); ++i)
    {
        _slotViews[i]->updateView(slots[i]);
    }
}

CardSlotView* MainAreaView::getSlotView(int index) const
{
    if (index < 0 || index >= static_cast<int>(_slotViews.size())) return nullptr;
    return _slotViews[index];
}

void MainAreaView::setSlotTapCallback(const SlotTapCallback& cb)
{
    _onSlotDoubleTap = cb;
}

void MainAreaView::setDragStartCallback(const DragStartCallback& cb)
{
    _onDragStart = cb;
}

void MainAreaView::setDragMoveCallback(const DragMoveCallback& cb)
{
    _onDragMove = cb;
}

void MainAreaView::setDragEndCallback(const DragEndCallback& cb)
{
    _onDragEnd = cb;
}

void MainAreaView::updateHighlights(const std::vector<bool>& highlightStates)
{
    for (int i = 0; i < static_cast<int>(_slotViews.size()); ++i)
    {
        auto* slotView = _slotViews[i];
        if (slotView->cardCount() == 0 || !slotView->isTopCardFaceUp())
        {
            slotView->setHighlight(false);
            continue;
        }

        const bool highlight = i < static_cast<int>(highlightStates.size()) && highlightStates[i];
        slotView->setHighlight(highlight);
    }
}
