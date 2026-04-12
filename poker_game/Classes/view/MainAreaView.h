#ifndef POKER_GAME_MAIN_AREA_VIEW_H
#define POKER_GAME_MAIN_AREA_VIEW_H

#include "CardSlotView.h"
#include "CardSlot.h"
#include "LayoutConfig.h"
#include <vector>
#include <functional>

class MainAreaView : public cocos2d::Node
{
public:
    static MainAreaView* create();

    bool init() override;

    // 从布局配置创建 slot 视图
    void setupFromLayout(const LayoutConfig& layout);

    // 获取总 slot 数
    int slotCount() const { return static_cast<int>(_slotViews.size()); }

    // 根据数据刷新所有视图
    void updateAllSlots(const std::vector<CardSlot>& slots);
    CardSlotView* getSlotView(int index) const;

    // 双击回调（替代原来的单击回调）
    using SlotTapCallback = std::function<void(int slotIndex)>;
    void setSlotTapCallback(const SlotTapCallback& cb);

    // 拖动回调
    using DragStartCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragMoveCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragEndCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    void setDragStartCallback(const DragStartCallback& cb);
    void setDragMoveCallback(const DragMoveCallback& cb);
    void setDragEndCallback(const DragEndCallback& cb);

    void updateHighlights(const std::vector<bool>& highlightStates);

private:
    std::vector<CardSlotView*> _slotViews;
    SlotTapCallback _onSlotDoubleTap;
    DragStartCallback _onDragStart;
    DragMoveCallback _onDragMove;
    DragEndCallback _onDragEnd;

    float _areaWidth;
    float _areaHeight;
};

#endif
