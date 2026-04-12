#ifndef POKER_GAME_CARD_SLOT_VIEW_H
#define POKER_GAME_CARD_SLOT_VIEW_H

#include "model/CardSlot.h"
#include "PokerCardView.h"
#include "PokerCard.h"
#include <vector>
#include <functional>
#include <chrono>

class CardSlotView : public cocos2d::Node
{
public:
    static CardSlotView* create(int slotIndex);

    bool initWithSlotIndex(int slotIndex);

    // 根据数据刷新视图
    void updateView(const CardSlot& slot);

    // 获取顶部卡牌视图
    PokerCardView* getTopCardView() const;

    // 获取顶部牌的世界坐标位置
    cocos2d::Vec2 getTopCardWorldPosition() const;

    // 顶部牌是否翻开
    bool isTopCardFaceUp() const;

    // 牌数
    int cardCount() const;

    // 双击回调（替代原来的单击回调）
    using TapCallback = std::function<void(int slotIndex)>;
    void setTapCallback(const TapCallback& cb);

    // 拖动回调：开始拖动、拖动中、释放
    using DragStartCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragMoveCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    using DragEndCallback = std::function<void(int slotIndex, const cocos2d::Vec2& worldPos)>;
    void setDragStartCallback(const DragStartCallback& cb);
    void setDragMoveCallback(const DragMoveCallback& cb);
    void setDragEndCallback(const DragEndCallback& cb);

    // 高亮
    void setHighlight(bool highlight);

    // 动画：将拖动的卡牌平滑回到原位
    void animateTopCardBack(cocos2d::CallFunc* callback = nullptr);

private:
    int _slotIndex;
    std::vector<PokerCardView*> _cardViews;
    TapCallback _onDoubleTap;
    DragStartCallback _onDragStart;
    DragMoveCallback _onDragMove;
    DragEndCallback _onDragEnd;

    const float _cardOverlapY = 9.0f;

    // 拖动状态
    bool _isDragging = false;
    bool _touchStartedOnTopCard = false;
    cocos2d::Vec2 _dragOffset;          // 触摸点与卡牌中心的偏移
    cocos2d::Vec2 _topCardOriginalPos;   // 拖动前卡牌原始位置
    float _topCardOriginalZOrder = 0;

    // 双击检测
    std::chrono::steady_clock::time_point _lastTapTime;
    int _slotIndexOfLastTap = -1;

    void clearCardViews();
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    bool hitTestTopCard(const cocos2d::Vec2& worldPos);
};

#endif
