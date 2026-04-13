/**
 * @file CardSlotView.h
 * @brief 卡槽视图头文件。
 *
 * 主要功能:
 *   - 管理槽位中的卡牌视图列表
 *   - 转发点击/拖拽事件给外部回调
 *   - 使用 InputDetector 做拖拽和双击判定
 */
#ifndef POKER_GAME_CARD_SLOT_VIEW_H
#define POKER_GAME_CARD_SLOT_VIEW_H

#include "model/CardSlot.h"
#include "PokerCardView.h"
#include "InputDetector.h"
#include "PokerCard.h"
#include <vector>
#include <functional>

// 主牌区槽位视图：展示一列叠放的牌并处理点击/拖拽交互。
class CardSlotView : public cocos2d::Node
{
public:
    static CardSlotView* create(int slotIndex);

    bool initWithSlotIndex(int slotIndex);

    // 根据槽位数据重建视图。
    void updateView(const CardSlot& slot);

    PokerCardView* getTopCardView() const;
    cocos2d::Vec2 getTopCardWorldPosition() const;
    bool isTopCardFaceUp() const;
    int cardCount() const;

    // 双击回调。
    using TapCallback = std::function<void(int slotIndex)>;
    void setTapCallback(const TapCallback& cb);

    // 拖动回调。
    using DragStartCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragMoveCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragEndCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    void setDragStartCallback(const DragStartCallback& cb);
    void setDragMoveCallback(const DragMoveCallback& cb);
    void setDragEndCallback(const DragEndCallback& cb);

    void setHighlight(bool highlight);
    void animateTopCardBack(cocos2d::CallFunc* callback = nullptr);

private:
    int _slotIndex;
    std::vector<PokerCardView*> _cardViews;
    TapCallback _onDoubleTap;
    DragStartCallback _onDragStart;
    DragMoveCallback _onDragMove;
    DragEndCallback _onDragEnd;

    // 拖动状态。
    bool _isDragging = false;
    bool _touchStartedOnTopCard = false;
    cocos2d::Vec2 _dragOffset;
    cocos2d::Vec2 _topCardOriginalPos;
    float _topCardOriginalZOrder = 0;

    // 输入检测。
    InputDetector _inputDetector;

    void clearCardViews();
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    bool hitTestTopCard(const cocos2d::Vec2& worldPos);
};

#endif
