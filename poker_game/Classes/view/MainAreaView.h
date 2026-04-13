/**
 * @file MainAreaView.h
 * @brief 主牌区视图头文件。
 *
 * 主要功能:
 *   - 管理多个 CardSlotView
 *   - bindController: 绑定控制器
 *   - syncFrom: 从 GameState 同步卡槽数据
 */
#ifndef POKER_GAME_MAIN_AREA_VIEW_H
#define POKER_GAME_MAIN_AREA_VIEW_H

#include "CardSlotView.h"
#include "CardSlot.h"
#include "LayoutConfig.h"
#include <vector>
#include <functional>

// 主牌区整体视图：管理多个 CardSlotView，负责布局与交互转发。
class MainAreaView : public cocos2d::Node
{
public:
    // 创建主牌区视图节点。
    static MainAreaView* create();

    bool init() override;

    // 按布局配置创建所有 slot 视图。
    // layout: 布局定义（含槽位位置与数量）。
    void setupFromLayout(const LayoutConfig& layout);

    // 获取总 slot 数
    int slotCount() const { return static_cast<int>(_slotViews.size()); }

    // 根据数据刷新所有视图
    // slots: 槽位模型数组，与视图索引对齐。
    void updateAllSlots(const std::vector<CardSlot>& slots);
    // 获取指定槽位视图。
    // index: 槽位索引。
    CardSlotView* getSlotView(int index) const;

    // 槽位输入回调。
    using SlotTapCallback = std::function<void(int slotIndex)>;
    void setSlotTapCallback(const SlotTapCallback& cb);

    // 拖动回调
    using DragStartCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragMoveCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragEndCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    void setDragStartCallback(const DragStartCallback& cb);
    void setDragMoveCallback(const DragMoveCallback& cb);
    void setDragEndCallback(const DragEndCallback& cb);

    // 批量更新槽位高亮状态，索引与槽位对齐。
    void updateHighlights(const std::vector<bool>& highlightStates);

private:
    std::vector<CardSlotView*> _slotViews; // 主牌区全部槽位视图，索引与 slot id 对齐
    SlotTapCallback _onSlotDoubleTap;
    DragStartCallback _onDragStart;
    DragMoveCallback _onDragMove;
    DragEndCallback _onDragEnd;

    float _areaWidth;  // 设计分辨率宽
    float _areaHeight; // 设计分辨率高
};

#endif
