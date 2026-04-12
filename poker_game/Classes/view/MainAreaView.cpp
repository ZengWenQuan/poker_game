#include "MainAreaView.h"
#include "PokerCardView.h"
#include "config/GlobalConfig.h"

USING_NS_CC;

// 创建主牌区视图。
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

// 初始化尺寸与锚点。
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

// 按布局创建全部槽位视图并绑定回调。
void MainAreaView::setupFromLayout(const LayoutConfig& layout)
{
    // 切布局时直接重建全部槽位视图，避免遗留旧布局节点。
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

        // MainAreaView 只做输入转发，不直接处理业务逻辑。
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

        // 旋转槽位时锚点切到左下角，保证和布局几何计算口径一致。
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

        // cocos 的 zOrder 越大越靠前，因此这里把 layer 反向映射。
        addChild(slotView, maxLayer - slotLayout.layer);

        _slotViews.push_back(slotView);
    }
}

// 根据状态层数据批量刷新槽位。
void MainAreaView::updateAllSlots(const std::vector<CardSlot>& slots)
{
    for (int i = 0; i < static_cast<int>(slots.size()) && i < static_cast<int>(_slotViews.size()); ++i)
    {
        _slotViews[i]->updateView(slots[i]);
    }
}

// 获取指定索引的槽位视图。
CardSlotView* MainAreaView::getSlotView(int index) const
{
    if (index < 0 || index >= static_cast<int>(_slotViews.size())) return nullptr;
    return _slotViews[index];
}

// 设置双击槽位回调。
void MainAreaView::setSlotTapCallback(const SlotTapCallback& cb)
{
    _onSlotDoubleTap = cb;
}

// 设置拖拽开始回调。
void MainAreaView::setDragStartCallback(const DragStartCallback& cb)
{
    _onDragStart = cb;
}

// 设置拖拽移动回调。
void MainAreaView::setDragMoveCallback(const DragMoveCallback& cb)
{
    _onDragMove = cb;
}

// 设置拖拽结束回调。
void MainAreaView::setDragEndCallback(const DragEndCallback& cb)
{
    _onDragEnd = cb;
}

// 批量更新高亮状态，索引与槽位对齐。
void MainAreaView::updateHighlights(const std::vector<bool>& highlightStates)
{
    // 高亮只关心“当前可操作的顶牌”。
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
